/*
 * Copyright (C) 2018 Red Hat, Inc.
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <algorithm>
#include <set>
#include <sstream>

extern "C" {
#include <solv/poolarch.h>
#include <solv/solver.h>
}

#include "ModulePackageContainer.hpp"
#include "libdnf/utils/utils.hpp"
#include "libdnf/utils/File.hpp"
#include "libdnf/dnf-sack-private.hpp"
#include <functional>
#include <../sack/query.hpp>

static std::string
stringFormater(std::string imput)
{
    return imput.empty() ? "*" : imput;
}

class ModulePackageContainer::Impl {
public:
    ~Impl();
    std::unique_ptr<libdnf::IdQueue> moduleSolve(
        const std::vector<std::shared_ptr<ModulePackage>> & modules);
private:
    friend struct ModulePackageContainer;
    std::map<Id, std::shared_ptr<ModulePackage>> modules;
    DnfSack * moduleSack;
    Map * activatedModules{nullptr};
    std::string installRoot;
};

ModulePackageContainer::ModulePackageContainer(bool allArch, std::string installRoot,
    const char * arch) : pImpl(new Impl)
{
    pImpl->moduleSack = dnf_sack_new();
    if (allArch) {
        dnf_sack_set_all_arch(pImpl->moduleSack, TRUE);
    } else {
        dnf_sack_set_arch(pImpl->moduleSack, arch, NULL);
    }
    Pool * pool = dnf_sack_get_pool(pImpl->moduleSack);
    repo_create(pool, "available");
    pImpl->installRoot = installRoot;
}

ModulePackageContainer::~ModulePackageContainer() = default;

ModulePackageContainer::Impl::~Impl()
{
    if (activatedModules) {
        map_free(activatedModules);
        delete activatedModules;
    }
    g_object_unref(moduleSack);
}

void
ModulePackageContainer::add(const std::string &fileContent)
{
    Pool * pool = dnf_sack_get_pool(pImpl->moduleSack);
    auto metadata = ModuleMetadata::metadataFromString(fileContent);
    Repo * r;
    Id id;

    FOR_REPOS(id, r) {
        if (strcmp(r->name, "available") == 0) {
            for (auto data : metadata) {
                std::shared_ptr<ModulePackage> modulePackage(new ModulePackage(
                    pImpl->moduleSack, r, data));
                pImpl->modules.insert(std::make_pair(modulePackage->getId(), modulePackage));
            }
            return;
        }
    }
}

Id
ModulePackageContainer::addPlatformPackage(const std::string& osReleasePath,
    const char* platformModule)
{
    return ModulePackage::createPlatformSolvable(pImpl->moduleSack, osReleasePath,
        pImpl->installRoot, platformModule);
}

void ModulePackageContainer::createConflictsBetweenStreams()
{
    // TODO Use libdnf::Query for filtering
    for (const auto &iter : pImpl->modules) {
        const auto &modulePackage = iter.second;

        for (const auto &innerIter : pImpl->modules) {
            if (modulePackage->getName() == innerIter.second->getName()
                && modulePackage->getStream() != innerIter.second->getStream()) {
                modulePackage->addStreamConflict(innerIter.second);
            }
        }
    }
}

std::shared_ptr<ModulePackage> ModulePackageContainer::getModulePackage(Id id)
{
    return pImpl->modules[id];
}

std::vector<std::shared_ptr<ModulePackage>>
ModulePackageContainer::requiresModuleEnablement(const libdnf::PackageSet & packages)
{
    auto activatedModules = pImpl->activatedModules;
    if (!activatedModules) {
        return {};
    }
    std::vector<std::shared_ptr<ModulePackage>> output;
    libdnf::Query baseQuery(packages.getSack());
    baseQuery.addFilter(HY_PKG, HY_EQ, &packages);
    baseQuery.apply();
    libdnf::Query testQuery(baseQuery);
    auto modules = pImpl->modules;
    auto moduleMapSize = dnf_sack_get_pool(pImpl->moduleSack)->nsolvables;
    for (Id id = 0; id < moduleMapSize; ++id) {
        if (!MAPTST(activatedModules, id)) {
            continue;
        }
        auto module = modules[id];
        if (module->isEnabled()) {
            continue;
        }
        auto includeNEVRAs = module->getArtifacts();
        std::vector<const char *> includeNEVRAsCString(includeNEVRAs.size() + 1);
        transform(includeNEVRAs.begin(), includeNEVRAs.end(), includeNEVRAsCString.begin(), std::mem_fn(&std::string::c_str));
        testQuery.queryUnion(baseQuery);
        testQuery.addFilter(HY_PKG_NEVRA_STRICT, HY_EQ, includeNEVRAsCString.data());
        if (testQuery.empty()) {
            continue;
        }
        output.push_back(module);
    }
    return output;
}

void ModulePackageContainer::enable(const std::string &name, const std::string &stream)
{
    for (const auto &iter : pImpl->modules) {
        auto modulePackage = iter.second;
        if (modulePackage->getName() == name && modulePackage->getStream() == stream) {
            modulePackage->enable();
        }
    }
}

std::unique_ptr<libdnf::IdQueue>
ModulePackageContainer::Impl::moduleSolve(const std::vector<std::shared_ptr<ModulePackage>> & modules)
{
    if (modules.empty()) {
        return {};
    }
    Pool * pool = dnf_sack_get_pool(moduleSack);
    pool_createwhatprovides(pool);
    std::vector<Id> solvedIds;
    libdnf::IdQueue job;
    for (const auto &module : modules) {
        std::ostringstream ss;
        ss << "module(" << module->getName() << ":" << module->getStream() << ":" << module->getVersion() << ")";
        Id dep = pool_str2id(pool, ss.str().c_str(), 1);
        job.pushBack(SOLVER_SOLVABLE_PROVIDES | SOLVER_INSTALL | SOLVER_WEAK, dep);
    }
    auto solver = solver_create(pool);
    solver_solve(solver, job.getQueue());
    auto transaction = solver_create_transaction(solver);
    // TODO Use Goal to allow debuging

    std::unique_ptr<libdnf::IdQueue> installed(new libdnf::IdQueue);
    transaction_installedresult(transaction, installed->getQueue());
    transaction_free(transaction);
    solver_free(solver);
    return installed;
}

std::vector<std::shared_ptr<ModulePackage>>
ModulePackageContainer::query(libdnf::Nsvcap& moduleNevra)
{
    // Alternativally a search using module provides could be performed
    std::vector<std::shared_ptr<ModulePackage>> result;
    libdnf::Query query(pImpl->moduleSack);
    // platform modules are installed and not in modules std::Map.
    query.addFilter(HY_PKG_REPONAME, HY_NEQ, HY_SYSTEM_REPO_NAME);
    std::ostringstream ss;
    ss << stringFormater(moduleNevra.getName()) << ":" << stringFormater(moduleNevra.getStream());
    ss << ":" << stringFormater(std::to_string(moduleNevra.getVersion())) << ":";
    ss << stringFormater(moduleNevra.getContext());
    query.addFilter(HY_PKG_NAME, HY_GLOB, ss.str().c_str());
    auto arch = moduleNevra.getArch();
    if (!arch.empty()) {
        query.addFilter(HY_PKG_ARCH, HY_GLOB, arch.c_str());
    }
    auto pset = query.runSet();
    Id moduleId = -1;
    while ((moduleId = pset->next(moduleId)) != -1) {
        result.push_back(pImpl->modules.at(moduleId));
    }
    return result;
}

std::vector<std::shared_ptr<ModulePackage>>
ModulePackageContainer::query(std::string subject)
{
    // Alternativally a search using module provides could be performed
    std::vector<std::shared_ptr<ModulePackage>> result;
    libdnf::Query query(pImpl->moduleSack);
    // platform modules are installed and not in modules std::Map.
    query.addFilter(HY_PKG_REPONAME, HY_NEQ, HY_SYSTEM_REPO_NAME);
    std::ostringstream ss;
    ss << "module(" << subject << ")";
    query.addFilter(HY_PKG_PROVIDES, HY_GLOB, ss.str().c_str());
    auto pset = query.runSet();
    Id moduleId = -1;
    while ((moduleId = pset->next(moduleId)) != -1) {
        result.push_back(pImpl->modules.at(moduleId));
    }
    return result;
}

void ModulePackageContainer::resolveActiveModulePackages(const std::map<std::string, std::string> &defaultStreams)
{
    Pool * pool = dnf_sack_get_pool(pImpl->moduleSack);
    std::vector<std::shared_ptr<ModulePackage>> packages;

    // Use only Enabled or Default modules for transaction
    for (const auto &iter : pImpl->modules) {
        auto module = iter.second;

        bool hasDefaultStream;
        try {
            hasDefaultStream = defaultStreams.at(module->getName()) == module->getStream();
        } catch (std::out_of_range &exception) {
            hasDefaultStream = false;
            // TODO logger.debug(exception.what())
        }
        if (module->isEnabled()) {
            packages.push_back(module);
        } else if (hasDefaultStream) {
            module->setState(ModulePackage::ModuleState::DEFAULT);
            packages.push_back(module);
        }
    }
    auto ids = pImpl->moduleSolve(packages);
    if (!ids || ids->size() == 0) {
        return;
    }
    if (pImpl->activatedModules) {
        map_free(pImpl->activatedModules);
    } else {
        pImpl->activatedModules = new Map;
    }
    map_init(pImpl->activatedModules, pool->nsolvables);
    auto activatedModules = pImpl->activatedModules;
    for (int i = 0; i < ids->size(); ++i) {
        Id id = (*ids)[i];
        auto solvable = pool_id2solvable(pool, id);
        // TODO use Goal::listInstalls() to not requires filtering out Platform
        if (strcmp(solvable->repo->name, HY_SYSTEM_REPO_NAME) == 0)
            continue;
        MAPSET(activatedModules, id);
    }
}

bool ModulePackageContainer::isModuleActive(Id id)
{
    if (pImpl->activatedModules) {
        return MAPTST(pImpl->activatedModules, id);
    }
    return false;
}


std::vector<std::shared_ptr<ModulePackage>> ModulePackageContainer::getModulePackages()
{
    std::vector<std::shared_ptr<ModulePackage>> values;
    auto modules = pImpl->modules;
    std::transform(std::begin(modules), std::end(modules), std::back_inserter(values),
                   [](const std::map<Id, std::shared_ptr<ModulePackage>>::value_type &pair){ return pair.second; });

    return values;
}