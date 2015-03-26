/*
  Copyright (c) 2014 Google Inc.
  Copyright (c) 2014, 2015 MariaDB Corporation

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/**
  Example key management plugin. It demonstrates how to return
  keys on request, how to change them. That the engine can have
  different pages in the same tablespace encrypted with different keys
  and what the background re-encryption thread does.

  THIS IS AN EXAMPLE ONLY! ENCRYPTION KEYS ARE HARD-CODED AND *NOT* SECRET!
  DO NOT USE THIS PLUGIN IN PRODUCTION! EVER!
*/

#include <my_global.h>
#include <my_pthread.h>
#include <my_aes.h>
#include <mysql/plugin_encryption_key_management.h>
#include <my_md5.h>
#include <my_rnd.h>
#include "sql_class.h"

/* rotate key randomly between 45 and 90 seconds */
#define KEY_ROTATION_MIN 45
#define KEY_ROTATION_MAX 90

static struct my_rnd_struct seed;
static unsigned int key_version = 0;
static unsigned int next_key_version = 0;
static pthread_mutex_t mutex;

static unsigned int
get_latest_key_version()
{
  uint now = time(0);
  pthread_mutex_lock(&mutex);
  if (now >= next_key_version)
  {
    key_version = now;
    unsigned int interval = KEY_ROTATION_MAX - KEY_ROTATION_MIN;
    next_key_version = now + KEY_ROTATION_MIN + my_rnd(&seed) * interval;
  }
  pthread_mutex_unlock(&mutex);

  return key_version;
}

static int
get_key(unsigned int version, unsigned char* dstbuf, unsigned buflen)
{
  unsigned char *dst = dstbuf;
  unsigned len = 0;
  for (; len + MD5_HASH_SIZE <= buflen; len += MD5_HASH_SIZE)
  {
    compute_md5_hash(dst, (const char*)&version, sizeof(version));
    dst += MD5_HASH_SIZE;
    version++;
  }
  if (len < buflen)
  {
    memset(dst, 0, buflen - len);
  }
  return 0;
}

static unsigned int has_key_func(unsigned int keyID)
{
  return true;
}

static unsigned int get_key_size(unsigned int keyID)
{
	return 16;
}

static int example_key_management_plugin_init(void *p)
{
  /* init */
  my_rnd_init(&seed, time(0), 0);
  get_latest_key_version();

  if (current_aes_dynamic_method == MY_AES_ALGORITHM_NONE)
  {
    sql_print_error("No encryption method choosen with --encryption-algorithm. "
                    "example_key_management_plugin disabled");
    return 1;
  }

  my_aes_init_dynamic_encrypt(current_aes_dynamic_method);

  pthread_mutex_init(&mutex, NULL);

  return 0;
}

static int example_key_management_plugin_deinit(void *p)
{
  pthread_mutex_destroy(&mutex);
  return 0;
}

struct st_mariadb_encryption_key_management example_key_management_plugin= {
  MariaDB_ENCRYPTION_KEY_MANAGEMENT_INTERFACE_VERSION,
  get_latest_key_version,
  has_key_func,
  get_key_size,
  get_key
};

/*
  Plugin library descriptor
*/
maria_declare_plugin(example_key_management_plugin)
{
  MariaDB_ENCRYPTION_KEY_MANAGEMENT_PLUGIN,
  &example_key_management_plugin,
  "example_key_management_plugin",
  "Jonas Oreland",
  "Example key management plugin",
  PLUGIN_LICENSE_GPL,
  example_key_management_plugin_init,
  example_key_management_plugin_deinit,
  0x0100 /* 1.0 */,
  NULL,	/* status variables */
  NULL,	/* system variables */
  "1.0",
  MariaDB_PLUGIN_MATURITY_EXPERIMENTAL
}
maria_declare_plugin_end;
