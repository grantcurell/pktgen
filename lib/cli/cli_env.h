/*-
 *   BSD LICENSE
 *
 *   Copyright(c) 2016-2017 Intel Corporation. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/* Create by: Keith Wiles @ intel.com */

#include <sys/queue.h>

#ifndef _CLI_ENV_H_
#define _CLI_ENV_H_

/**
 * @file
 * RTE Command line interface
 *
 */

#ifdef __cplusplus
extern "C" {
#endif
struct cli;

struct env_node {
	TAILQ_ENTRY(env_node) next;
	const char *var;
	const char *val;
};

struct cli_env {
    TAILQ_HEAD(, env_node) head;     /**< link list of vars */
    int count;
};

/**
 * Create a environment for the cli
 *
 * @return
 *   NULL on error or cli_env pointer
 */
struct cli_env *cli_env_create(void);

/**
 * Delete the environment for the CLI
 *
 * @param cli
 *   The pointer to the enviroment structure
 */
void cli_env_destroy(struct cli_env *env);

/**
 * Set a environment variable for the CLI
 *
 * @param env
 *   The cli_env pointer
 * @param var
 *   Pointer to the variable name const string
 * @param val
 *   Pointer to the string assigned to the variable
 * @return
 *   0 is OK was added or replaced or -1 if not valid
 */
int cli_env_set(struct cli_env *env, const char *var, const char *val);

/**
 * Get the environment variable from the cli
 *
 * @param env
 *   The cli_env pointer
 * @param var
 *   The const string variable name
 * @return
 *   NULL if not found or the const string
 */
const char *cli_env_get(struct cli_env *env, const char *var);

/**
 * Remove the environment variable from the cli
 *
 * @param env
 *   The cli_env pointer
 * @param var
 *   The const string variable name
 * @return
 *   0 is OK or -1 if not found.
 */
int cli_env_remove(struct cli_env *env, const char *var);

/**
 * Do enviroment variable subsitution on the line.
 *
 * @param env
 *   Pointer to the enviroment structure
 * @param line
 *   Pointer to the line to parse
 * @param sz
 *   Number of total characters the line can hold.
 * @return
 *   N/A
 */
void cli_env_substitution(struct cli_env *env, char *line, int sz);

/**
 * Get the number of variables in the environment
 *
 * @param env
 *   Pointer to environment structure
 * @return
 *   Number of environment variables
 */
static inline int
cli_env_count(struct cli_env *env)
{
	return env->count;
}

/**
 * Get all of the environment variables
 *
 * @param env
 *   Pointer to environment list
 * @param list
 *   Array of env_node pointers to be returned
 * @param max_size
 *   Max size of the list array
 */
int cli_env_get_all(struct cli_env *env, struct env_node **list, int max_size);


#ifdef __cplusplus
}
#endif

#endif /* _CLI_ENV_H_ */
