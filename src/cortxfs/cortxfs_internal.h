/*
 * Filename:         cortxfs_internal.h
 * Description:      CORTXFS file system internal APIs
 *
 * Copyright (c) 2020 Seagate Technology LLC and/or its Affiliates
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Affero General Public License for more details.
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * For any questions about this software or licensing,
 * please email opensource@seagate.com or cortx-questions@seagate.com. 
 */

#ifndef _CFS_INTERNAL_H
#define _CFS_INTERNAL_H

#include <dstore.h>
#include <nsal.h>
#include "cortxfs.h"
#include "kvnode.h"

enum cfs_sys_attr_type
{
   CFS_SYS_ATTR_SYMLINK = 1,
   CFS_SYS_ATTR_INO_NUM_GEN,
   CFS_SYS_ATTR_MAX
};

#define CFS_ROOT_INODE_NUM_GEN_START (CFS_ROOT_INODE + 1)

/**
 * A kvnode is identified by a 128 bit node id. However, currently cortxfs uses
 * 64 bit inodes in it's apis to identify the entities. The following apis
 * are a temporary arrangement to convert 64 bit inodes to 128 bit node ids.
 * @TODO: Cleanup these apis when cortxfs filehandle (or equivalent) is
 * implemented. The filehandles would eventually use 128bit unique fids.
 */
static inline int node_id_to_ino(const node_id_t *nid, cfs_ino_t *ino)
{
	dassert(nid != NULL);
	dassert(ino != NULL);

	*ino = nid->f_hi;
	return 0;
}

static inline int ino_to_node_id(const cfs_ino_t *ino, node_id_t *nid)
{
	dassert(ino != NULL);
	dassert(nid != NULL);

	nid->f_hi = *ino;
	nid->f_lo = 0;

	return 0;
}

/** Set the extstore object identifier (kfid) with cortxfs inode as the key */
int cfs_set_ino_oid(struct cfs_fs *cfs_fs, cfs_ino_t *ino, dstore_oid_t *oid);

/** Get the extstore object identifier for the passed cortxfs inode */
int cfs_ino_to_oid(struct cfs_fs *cfs_fs, const cfs_ino_t *ino, dstore_oid_t *oid);

/** Delete the ino-kfid key-val pair from the kvs. Called during unlink/rm. */
int cfs_del_oid(struct cfs_fs *cfs_fs, const cfs_ino_t *ino);

/* Initialize the kvnode with given parameters
 *
 * @param[in] node *    - Kvnode pointer which will be initialized using kvnode
 *                        API to hold node information
 * @param[in] tree *    - Kvtree pointer which will be stored in kvnode on init
 * @param[in] ino *     - Inode of a file - file identifier
 * @param[in] bufstat * - Stat attribute buffer, stored in kvnode
 *
 * @return - 0 on sucess on failure error code given by kvnode APIs
 */
int cfs_kvnode_init(struct kvnode *node, struct kvtree *tree,
		    const cfs_ino_t *ino, const struct stat *bufstat);

/* Load kvnode for given file identifier from disk
 *
 * @param[in] node *    - Kvnode pointer which will be initialized using kvnode
 *                        API to hold node information
 * @param[in] tree *    - Kvtree pointer which will be stored in kvnode on init
 * @param[in] ino *     - Inode of a file - file identifier
 *
 * @return - 0 on sucess on failure error code given by kvnode APIs
 */
int cfs_kvnode_load(struct kvnode *node, struct kvtree *tree,
		    const cfs_ino_t *ino);

/* Extract the stat attribute for particular file held by given kvnode
 *
 * @param[in] node *     - Kvnode pointer which is already initialzed and having
 *                         stat attributes
 * @param[in] bufstat ** - Stat attribute buffer pointer to store stat attribute
 *
 * @return - 0 on sucess on failure error code given by kvnode APIs
 */
int cfs_get_stat(const struct kvnode *node, struct stat **bufstat);

/* Delete the stat associated with particular file inode held by given kvnode
 *
 * @param[in] node *    - Kvnode pointer which is initialized and needs to be
 *                        deleted
 *
 * @return - 0 on sucess on failure error code given by kvnode APIs
 */
int cfs_del_stat(struct kvnode *node);

/* Update the stat hold by given kvnode for given flag and re-store it back to
 * the disk
 *
 * @param[in] node *   - Kvnode pointer which is already initialzed and having
 *                       stat attributes
 * @param[in] flags    - Flag mask for which stats needs to be updated
 *
 * @return - 0 on sucess on failure error code given by kvnode APIs
 */
int cfs_update_stat(struct kvnode *node, int flags);
/*
 * This API will store any type of system attributes associated with
 * given node
 *
 * @param[in] node *    - node for which system attributes to be stored
 * @param[in] value    - Buffer which needs to be be stored
 * @param[in] attr_type - Type of system attirbutes to be stored
 *
 * @return - 0 on success else error code return by kvnode APIs
 */
int cfs_set_sysattr(const struct kvnode *node, const buff_t value,
		    enum cfs_sys_attr_type attr_type);

/*
 * This API will fetch any type of system attributes associated with
 * given node
 *
 * @param[in] node *    - node for which system attributes to be fetched
 * @param[in] value *   - Ponter to buffer to store attributes
 * @param[in] attr_type - Type of system attirbutes to be fetched
 *
 * @return - 0 on success else error code return by kvnode APIs
 */
int cfs_get_sysattr(const struct kvnode *node, buff_t *value,
		    enum cfs_sys_attr_type attr_type);

/*
 * This API will delete any type of system attributes associated with
 * given node
 *
 * @param[in] node *    - node for which system attributes to be deleted
 * @param[in] attr_type - Type of system attirbutes to be deleted
 *
 * @return - 0 on success else error code return by kvnode APIs
 */
int cfs_del_sysattr(const struct kvnode *node,
		    enum cfs_sys_attr_type attr_type);
#endif
