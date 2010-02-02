/* 
  myfs_dir.c 
  Directory Bookkeeping stuff.
 */
#include "myfs.h"

static int myfs_readdir(struct file * filp, void * dirent, filldir_t filldir)
{
  struct inode * inode = filp->f_path.dentry->d_inode;
  unsigned long blksize = inode->i_sb->s_blocksize;
  struct myfs_inode * minode = myfs_i(inode);
  int i =0;
  unsigned size = sizeof(struct myfs_dentry );

  while(minode->mi_data_blocks[i] !=NULL)
    {
      void * offset = minode->mi_data_blocks[i];
      void * limit = offset + blksize - size ; 

      while((offset + size) < limit)
	{
	  struct myfs_dentry * de = (struct myfs_dentry *) offset;
	  if(de->inode == 0) 	/* No more dirents. */
	    goto done;
	  
	  if(filldir(dirent, de->name, de->len, offset, 
		     de->inode, DT_UNKNOWN))
	    goto done;

	  offset +=size; 	/* goto next entry. */

	}
      i++;
    }
      
 done:
  return 0;
}


int myfs_sync_file(struct file* file, struct dentry * dentry, int datasync)
{
  /* Since we are an in memory system, we dont need to do anything here. */
  return 0;
}

const struct file_operations myfs_dir_operations = {
  .read = generic_read_dir,
  .readdir = myfs_readdir,
  .fsync = myfs_sync_file,
};

/* Write link to directory. */
int myfs_add_link(struct dentry *dentry, struct inode * inode)
{
  struct inode *dir = dentry->d_parent->d_inode;
  struct myfs_dentry mydirent;
  size_t count,write;
  
  mydirent.inode = inode->i_ino;
  mydirent.len = dentry->d_name.len;
  memcpy(mydirent.name,dentry->d_name.name,mydirent.len);
  
  count = 0 ;
  write = sizeof(mydirent);
  
  /* Write to parent inode. */
  do{
    count = myfs_write_data(dir,&mydirent + count,write,dir->i_size);
    if(count == -ENOMEM)
      return count;

    write = write-count;
  }while(count < write);
  
  dir->i_mtime = dir->i_ctime = CURRENT_TIME_SEC;

  return count;
}

ino_t myfs_inode_by_name(struct dentry * dentry)
{
  struct myfs_inode * dir ;
  const char * name = dentry->d_name.name;
  int namelen = dentry->d_name.len;
  int numpages,block,read = 0,i;
  unsigned int size;

  dir = myfs_i(dentry->d_parent->d_inode);
  size = dir->vfs_inode.i_size;
  numpages = (int) (size / (dir->vfs_inode.i_sb->s_blocksize));
  
  if(size == 0 || size < sizeof(struct myfs_dentry))
    return NULL;

  /* Search through all the pages looking for the correct block. */
  for(block = 0; block <= numpages; block ++)
    {
      struct myfs_dentry * dentry= dir->mi_data_blocks[block];
      for(i=0; read < size; i++)
	{
	  if(dentry[i].len == namelen && !memcmp(name, dentry[i].name, dentry[i].len))
	    return dentry[i].inode;
	  
	  read += sizeof(struct myfs_dentry);
	}
    }

  dbg("Name:%s not found in directory:%s", name, dentry->d_parent->d_name.name);
  return 0;
}

int myfs_make_empty(struct inode * inode, struct inode * dir)
{
  struct myfs_inode * minode = myfs_i(inode);
  unsigned long size = inode->i_sb->s_blocksize;
  struct myfs_dentry * de;

  /* Allocate memory for the first block. */

  minode->mi_data_blocks[0]=kmalloc(size, GFP_KERNEL);
  if(!minode->mi_data_blocks[0])
    {
      dbg("Did not get memory!!");
      return -ENOMEM;
    }
  
  memset(minode->mi_data_blocks[0],0,size);
  
  de = (struct myfs_dentry*) minode->mi_data_blocks[0];
  de->inode = minode->vfs_inode.i_ino;
  strcpy(de->name,".");
  de->len = strlen(".");
  de = de + sizeof(struct myfs_dentry);
  de->inode = dir->i_ino;
  strcpy(de->name, "..");
  de->len = strlen("..");

  inode->i_size = 2 * sizeof(struct myfs_dentry);

  return 0;
}
