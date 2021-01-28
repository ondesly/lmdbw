//
//  db.cpp
//  lmdbw
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 28.01.2021.
//  Copyright © 2021 Dmitrii Torkhov. All rights reserved.
//

#include <liblmdb/lmdb.h>

#include "lmdbw/db.h"
#include "lmdbw/exception.h"

namespace {

    const MDB_dbi c_max_dbs = 64;
    const size_t c_map_size = 128UL * 1024UL * 1024UL * 1024UL; // 128 Gb
    const mdb_mode_t c_open_mode = 0664; // -rw-rw-r--

}

std::mutex lm::db::s_mutex;

std::unordered_map<std::string, MDB_env *> lm::db::s_envs;
std::unordered_map<std::string, MDB_dbi> lm::db::s_dbis;

lm::db::db(const lm::db::env_params &env_params, const lm::db::dbi_params &dbi_params) {
    const std::lock_guard<std::mutex> lock(s_mutex);

    // Environment

    const auto env_it = s_envs.find(env_params.path);
    if (env_it == s_envs.end()) {
        m_env = s_envs[env_params.path] = open_env(env_params);
    } else {
        m_env = env_it->second;
    }

    // DBI

    const auto dbi_key = env_params.path + dbi_params.name;
    const auto dbi_it = s_dbis.find(dbi_key);
    if (dbi_it == s_dbis.end()) {
        m_dbi = s_dbis[dbi_key] = open_dbi(m_env, dbi_params);
    } else {
        m_dbi = dbi_it->second;
    }
}

MDB_env *lm::db::open_env(const lm::db::env_params &env_params) {
    MDB_env *env;
    if (const auto rc = mdb_env_create(&env)) {
        mdb_env_close(env);
        throw lm::exception{"open_env::mdb_env_create", rc};
    }

    if (const auto rc = mdb_env_set_maxdbs(env, env_params.max_dbs == 0 ? c_max_dbs : env_params.max_dbs)) {
        mdb_env_close(env);
        throw lm::exception{"open_env::mdb_env_set_maxdbs", rc};
    }

    if (const auto rc = mdb_env_set_mapsize(env, env_params.map_size == 0 ? c_map_size : env_params.map_size)) {
        mdb_env_close(env);
        throw lm::exception{"open_env::mdb_env_set_mapsize", rc};
    }

    if (const auto rc = mdb_env_open(env, env_params.path.c_str(), env_params.flags,
                                     env_params.open_mode == 0 ? c_open_mode : env_params.open_mode)) {
        mdb_env_close(env);
        throw lm::exception{"open_env::mdb_env_open", rc};
    }

    return env;
}

MDB_dbi lm::db::open_dbi(MDB_env *env, const lm::db::dbi_params &dbi_params) {
    MDB_txn *txn;
    if (const auto rc = mdb_txn_begin(env, nullptr, 0, &txn)) {
        throw lm::exception{"open_dbi::mdb_txn_begin", rc};
    }

    MDB_dbi dbi;
    if (const auto rc = mdb_dbi_open(txn, dbi_params.name.c_str(), dbi_params.flags, &dbi)) {
        mdb_txn_abort(txn);
        throw lm::exception{"open_dbi::mdb_dbi_open", rc};
    }

    if (const auto rc = mdb_txn_commit(txn)) {
        mdb_dbi_close(env, dbi);
        throw lm::exception{"open_dbi::mdb_txn_commit", rc};
    }

    return dbi;
}

MDB_env *lm::db::get_env() const {
    return m_env;
}

MDB_dbi lm::db::get_dbi() const {
    return m_dbi;
}