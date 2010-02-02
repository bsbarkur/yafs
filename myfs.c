/**
   My First Trivial FS implementation.
   Will create a simple filesystem and a corresponding file.

   CopyLeft : Krishna G Pai.
 */

#include "myfs.h"
#include <asm/io.h>

/* File Operations .
   Right Now , We will support only one layer of file's. 
   Any number of files.
   But data in them will be limited to 1 Block.
*/


static void myfs_create_files(struct super_block *sb, struct dentry *root)
{
  /* Create One solitary file , right now. */
  //  myfs_create_file(sb, root, "Hello.txt",NULL );
}


/* Just copy inode data pointer to file, so we can */
static int myfs_open(struct inode * inode, struct file *filp)
{
  filp->private_data = inode->i_private;
  return 0;
}


static ssize_t myfs_read_file(struct file*filp, char __user *buf,
			      size_t count, loff_t *offset)
{
  struct inode * inode = filp->f_dentry->d_inode;
  ssize_t retval = -ENOMEM;
  unsigned long blksize = inode->i_sb->s_blocksize;
  unsigned long size = inode->i_sb->s_blocksize;
  int block,rest;
  struct myfs_inode * minode = myfs_i(inode);

  dbg("In Read.");

  if(*offset >= inode->i_size)
    goto out; 
  if(*offset + count >= inode->i_size)
    count = inode->i_size - *offset;

  block = (long)*offset / size; 
  rest = (long)*offset % size;
  
  /* Right now we support only direct blocks.. */
  if (block >= MYFS_DIRECT_BLOCKS) { /* We should honestly never get this. */
    dbg("Trying to access beyond the 40k boundary-- %d",offset);
    goto out; 
  }

  if( count > blksize - rest )
    count = blksize - rest;
  
  /* Reading from  a hole */
  if(!minode->mi_data_blocks[block])
    { 				
      dbg("read: Reading a zero block:%d",block);
      memset_io(buf,0,count);
      retval = 0;
      goto out;
    }
  
  if(copy_to_user(buf, minode->mi_data_blocks[block]+rest,count))
    {
      dbg("Copy to user failed.\n");
      retval = -EFAULT;
      goto out;
    }

 out:
  return retval;
}


/* Create the file operations structure. */

static ssize_t myfs_write_data_user(struct inode * inode, const char __user *buf,
				    size_t count, loff_t * offset)
{
  ssize_t retval = -ENOMEM;
  unsigned long size = inode->i_sb->s_blocksize;
  int block,rest;
  struct myfs_inode * minode = myfs_i(inode);

  block = (long)*offset / size; 
  rest = (long)*offset % size;
  
  /* Right now we support only direct blocks.. */
  if (block >= MYFS_DIRECT_BLOCKS) {
    dbg("Trying to access beyond the 40k boundary-- %d",offset);
    goto out; 
  }

  if(!minode->mi_data_blocks[block])
    {
      dbg("Allocating memory for block:%d",block);
      minode->mi_data_blocks[block] = kmalloc(size, GFP_KERNEL);
      if(!minode->mi_data_blocks[block])
	{
	  dbg("Did not get memory!!");
	  goto out;
	}
      memset(minode->mi_data_blocks[block],0,size);
    }

  /* Write only a 'size' at a time. */
  if( count > size - rest )
    count = size - rest;

  if(copy_from_user(minode->mi_data_blocks[block]+ rest, buf, count)) {
    retval = -EFAULT;
    goto out;
  }

  *offset += count;
  retval = count;

  if(inode->i_size < *offset)
    inode->i_size = *offset;
  
 out:
  return retval;

}

static ssize_t myfs_write_file (struct file *filp, const char __user *buf,
			       size_t count, loff_t *offset)
{
  struct inode * inode = filp->f_dentry->d_inode;

  return myfs_write_data_user(inode, buf, count, offset);
  
}



/* TODO: Make Thread safe. Use this for directory or kernelland write's. */
ssize_t myfs_write_data (struct inode * inode, void * data, size_t count,
			loff_t * offset)
{
  ssize_t retval = -ENOMEM;
  unsigned long size = inode->i_sb->s_blocksize;
  int block,rest;
  struct myfs_inode * minode = myfs_i(inode);

  block = (long)*offset / size; 
  rest = (long)*offset % size;
  
  /* Right now we support only direct blocks.. */
  if (block >= MYFS_DIRECT_BLOCKS) {
    dbg("Trying to access beyond the 40k boundary--%d",offset);
    goto out; 
  }

  if(!minode->mi_data_blocks[block])
    {
      dbg("Allocating memory for block:%d",block);
      minode->mi_data_blocks[block] = kmalloc(size, GFP_KERNEL);
      if(!minode->mi_data_blocks[block])
	{
	  dbg("Did not get memory!!");
	  goto out;
	}
      memset(minode->mi_data_blocks[block],0,size);
    }

  /* Write only a 'size' at a time. */
  if( count > size - rest )
    count = size - rest;

  memcpy(minode->mi_data_blocks[block] + rest , data, count);
  *offset += count;
  retval = count;

  if(inode->i_size < *offset)
    inode->i_size = *offset;
  
 out:
  return retval;
}

const struct file_operations myfs_file_operations = {
  .open = myfs_open,
  .read = myfs_read_file,
  .write = myfs_write_file,
};

const struct inode_operations myfs_file_inode_operations = {
  .truncate = myfs_truncate,
  .getattr = myfs_getattr,
};

