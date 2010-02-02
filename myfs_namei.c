/* 
   namei.c 
   
   Code related to directory operations.
 */

#include "myfs.h"


static int add_nondir(struct dentry * dentry, struct inode * inode)
{
  int err = myfs_add_link(dentry, inode);

  if(!err) {
    d_instantiate(dentry, inode);
    return 0;
  }

  inode_dec_link_count(inode);
  iput(inode);
  return err;

}

static int myfs_mknod(struct inode * dir, struct dentry * dentry, int mode,
		      dev_t rdev)
{
  /* Create a special device file. */
  int error = 0;
  struct inode * inode;

  if(!old_valid_dev(rdev))
    return -EINVAL;

  inode = myfs_new_inode(dir, &error);


  if(inode) {
      inode->i_mode = mode;
      myfs_set_inode(inode,rdev);
      error = myfs_add_link(dentry,inode);
    }

  return error;
}


static int myfs_create( struct inode * dir, struct dentry * dentry, int mode,
			struct nameidata * nd)
{
  /* Code to create a directory or regular file. */
  return myfs_create(dir, dentry, mode,0);
}

static struct dentry * myfs_lookup(struct inode * dir, struct dentry * dentry,
				   struct nameidata * nd)
{

  struct inode * inode = NULL;
  ino_t ino;

  dentry->d_op = dir->i_sb->s_root->d_op;

  if(dentry->d_name.len > 55)
    return ERR_PTR(-ENAMETOOLONG);

  ino = myfs_inode_by_name(dentry);

  if(ino) {
    
    inode = myfs_iget(dir->i_sb, ino);
    if(IS_ERR(inode))
      return ERR_CAST(inode);
  }

  d_add(dentry, inode);
  return NULL;
}





static int myfs_link(struct dentry * old_dentry, struct inode * dir, 
		     struct dentry * dentry)
{
  /* Create Hard link. */
  struct inode * inode = old_dentry->d_inode;

  inode->i_ctime = CURRENT_TIME_SEC;
  inode_inc_link_count(inode);
  atomic_inc(&inode->i_count);
  return add_nondir(dentry, inode);
}

static int myfs_mkdir(struct inode * dir, struct dentry * dentry, int mode)
{
  struct inode * inode;
  int err = -EMLINK;

  inode_inc_link_count(dir);
  inode = myfs_new_inode(dir,&err);
  if(!inode)
    goto out_dir;

  inode->i_mode = S_IFDIR| mode;
  if (dir->i_mode & S_ISGID)
    inode->i_mode |= S_ISGID;
  myfs_set_inode(inode, 0);

  inode_inc_link_count(inode);

  err = myfs_make_empty(inode, dir);
  if(err)
    goto out_fail;

  err = myfs_add_link(dentry, inode);
  if(err)
    goto out_fail;

  d_instantiate(dentry, inode);

 out:
  return err;

 out_fail:
  inode_dec_link_count(inode);
  inode_dec_link_count(inode);
  iput(inode);
 out_dir:
  inode_dec_link_count(dir);
  return err;
}

static int myfs_unlink(struct inode * dir, struct dentry * dentry)
{
  return 0;
}

static int myfs_rmdir(struct inode * dir, struct dentry * dentry)
{
  return 0;
}

static int myfs_rename(struct inode * old_dir, struct dentry * old_dentry,
		       struct inode * new_dir, struct dentry * new_dentry)
{

  return 0;
}

static int myfs_symlink(struct inode * dir, struct dentry * dentry,
			const char * symname)
{
  /*  Create Symbolic link . duh.. */
  return 0;
}

int myfs_getattr(struct vfsmount * mnt, struct dentry * dentry, 
			struct kstat * stat)
{
  struct inode * dir = dentry->d_parent->d_inode;
  struct super_block * sb = dir->i_sb;
  generic_fillattr(dentry->d_inode, stat);
  stat->blksize = sb->s_blocksize;
  return 0;
}

const struct inode_operations myfs_dir_inode_operations  = {
  .create = myfs_create,
  .lookup = myfs_lookup,
  .link = myfs_link,
  .unlink = myfs_unlink,
  .symlink = myfs_symlink,
  .mkdir = myfs_mkdir,
  .rmdir = myfs_rmdir,
  .mknod = myfs_mknod,
  .rename = myfs_rename,
  .getattr = myfs_getattr,
};
