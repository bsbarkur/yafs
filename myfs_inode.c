/* myfs_inode.c
   All inode related operations. 
*/

#include "myfs.h" 

static struct kmem_cache * myfs_inode_cachep;

static unsigned long myfs_get_inodenumber(struct super_block * sb)
{
  struct myfs_sbinfo *sbi = sb->s_fs_info;
  unsigned long inum;
  int i;

  //  mutex_lock(&(sb->s_lock));
  
  if(sbi->cur < sbi->len) { 
     inum = sbi->numarr[sbi->cur];
     sbi->numarr[sbi->cur] = 0;
     sbi->cur++;
  } else {
    /* Check if any inode numbers are back in the list. */
    
    for(i=0; (i < sbi->len) && (sbi->numarr[i] != 0); i++);
    
    if(i >= sbi->len){
      /* Ok..All zero's, so set the new inode numbers */
      for(i=0; i< sbi->len; i++)
	sbi->numarr[i] = sbi->maxnum ++;
      
      sbi->cur = 0;
      inum = sbi->numarr[sbi->cur];
      sbi->cur++;
    }else{
      /* 
	 Do not increment sbi->cur since we go on doing this till
         we run out of inode numbers.
      */
      inum = sbi->numarr[i];
    }
  }
  
  //mutex_unlock(&(sb->s_lock));
  return inum;
}

struct inode * myfs_new_inode(const struct inode* dir, int * error)
{
  /* Since we are using an in memory fs, we are not going to pool unused
     inodes.
  */
  struct super_block * sb = dir->i_sb;
  struct inode * inode =  new_inode(sb);
  struct myfs_inode *minode ;
  int i;

  if(!inode) {
    *error = -ENOMEM;
    return NULL;
  }
  
  inode->i_uid = current->fsuid;
  inode->i_gid = (dir->i_mode & S_ISGID) ? dir->i_gid : current->fsgid; 
  inode->i_ino = myfs_get_inodenumber(sb);
  inode->i_mtime = inode->i_atime = inode->i_ctime = CURRENT_TIME_SEC;
  inode->i_blocks = 0;
  inode->i_size = 0;

  minode = myfs_i(inode);
  
  for(i=0; i< MYFS_DIRECT_BLOCKS; i++)
    minode->mi_data_blocks[i]=NULL;

  insert_inode_hash(inode);
  mark_inode_dirty(inode);
  
  *error =0;
  return inode;
}


static void myfs_init_once(void * dat)
{
  struct myfs_inode * mi = (struct myfs_inode *) dat;
  inode_init_once(&mi->vfs_inode);
}

static int myfs_init_inodecache(void)
{
  myfs_inode_cachep = kmem_cache_create("myfs_inode_cache",
					   sizeof(struct myfs_inode),
					   0,
					   (SLAB_RECLAIM_ACCOUNT| SLAB_MEM_SPREAD),
					   myfs_init_once);
  if ( myfs_inode_cachep == NULL)
    return -ENOMEM;
  return 0;
}

static struct inode * myfs_alloc_inode ( struct super_block *sb)
{
  struct myfs_inode *mi;
  mi = (struct myfs_inode*) kmem_cache_alloc(myfs_inode_cachep,GFP_KERNEL);

  if (!mi)
    return NULL;
  return &mi->vfs_inode;
}

static void myfs_destroy_inode(struct inode *inode)
{
  
  kmem_cache_free(myfs_inode_cachep,
		  list_entry(inode, struct myfs_inode, vfs_inode));
}

static void myfs_destroy_inodecache(void)
{
  kmem_cache_destroy(myfs_inode_cachep);
}

/* TODO -- Implement statfs ! */

/* Since we are a completely in memory FS . All changes are written already. */
static int myfs_write_inode(struct inode * inode, int wait)
{
  return 0;
}

static void myfs_put_super(struct super_block *sb)
{
  /* Do all super block related work here. 
     Since we are an in memory FS , we would just need to make sure
     we free up memory.
  */

  return ;
}

static int myfs_remount(struct super_block * sb, int * flags, char * data)
{
  /* Not sure what to do here. Once you unmount you loose everything. */
  dbg( "In Remount" );
  return 0;
}


/* Super BLock operations. 
   Using generic operations.
 */

static void myfs_delete_inode(struct inode * inode)
{
  /*
    This is for reclaiming inodes which are no longer required.
    Do not free memory associated with inode. That will be done by
    destroy_inode(). 
  */
  /* truncate_inode_pages(...); 
     Set everything to zero. 
   */

}

void myfs_truncate(struct inode* inode)
{
  /* Truncate the data in the inode. */

}

static struct super_operations myfs_s_ops = {
  .alloc_inode = myfs_alloc_inode,
  .destroy_inode = myfs_destroy_inode,
  .write_inode = myfs_write_inode,
  .delete_inode = myfs_delete_inode,
  .put_super =  myfs_put_super,
  .statfs= simple_statfs, /* TODO */
  };


/* General Helper function to create inodes. */

 struct inode * myfs_iget(struct super_block * sb,
				unsigned long  ino )
{
  struct inode * inode = iget_locked(sb,ino);
  
  if(!inode)
    return ERR_PTR(-ENOMEM);

  if(!(inode->i_state & I_NEW))
    return inode;

  /* Set the inode. */
  /* This is unnecessary . This code should be handled at the directory level. */
  /* node->i_uid = current->fsuid; */
/*   node->i_gid = current->fsgid;  */
/*   node->i_atime = node->i_mtime = node->i_ctime = CURRENT_TIME; */
  
  /* To do file system specific stuff here. */
  return inode ;
}

/* This function should be called after a new node is created. */
void myfs_set_inode(struct inode * inode, dev_t rdev)
{
  /* TODO !! -- Adress Operations */
  if (S_ISREG(inode->i_mode)) {
    inode->i_op = &myfs_file_inode_operations;
    inode->i_fop = &myfs_file_operations;
  }else if (S_ISDIR(inode->i_mode)) {
    inode->i_op = &myfs_dir_inode_operations;
    inode->i_fop = &myfs_dir_operations;
  }else if (S_ISLNK(inode->i_mode)) {

  }else 
    init_special_inode(inode, inode->i_mode, rdev);
  
}


static int 
myfs_fill_super( struct super_block * sb,
		 void * data, int silent)
{
  struct inode * root;
  struct dentry * root_dentry ;
  struct myfs_sbinfo * sbi;
  int i;
  /* Since we are going to use Memory for the fs, 
   it makes sense to use one page as the block size.
  */

  /* 1. Setup Superblock. */
  sb->s_blocksize = PAGE_CACHE_SIZE;
  sb->s_blocksize_bits = PAGE_CACHE_SHIFT; 
  sb->s_magic = MYFS_MAGIC;
  sb->s_op = & myfs_s_ops ; //TODO : Confirm which all operations would be required.
  
  sbi = kzalloc(sizeof(struct myfs_sbinfo), GFP_KERNEL);
  if(!sbi)
    return -ENOMEM;
  
  sbi->numarr = kmalloc(sizeof(unsigned long) * MYFS_INODE_QUEUE, GFP_KERNEL);
  if(!sbi->numarr)
    goto out_sbi;

  for(i=0; i< MYFS_INODE_QUEUE; i++)
    sbi->numarr[i] = i + 2;
  sbi->len = MYFS_INODE_QUEUE;
  sbi->maxnum = MYFS_INODE_QUEUE + 2;
  sb->s_fs_info = sbi;
    
  root = new_inode(sb); 	

  if(! root)
    goto out;

  root->i_ino = 1;
  root->i_mode = S_IFDIR|0755;
  myfs_set_inode(root,0); 	/* TODO. */
  root->i_size = 0;

   /* 1.2 Make that Inode a directory. */
  root_dentry = d_alloc_root(root);
  if(! root_dentry)
    goto out_iput;

  if( myfs_make_empty(root, root))
    goto out_iput;

  sb->s_root = root_dentry;

  /*Makes file's for this system if necessary.*/
  /*  myfs_create_files(sb, root_dentry); */
  return 0;

 out_iput:
  iput(root);
 out_sbi:
  kfree(sbi);
 out: 
  return -ENOMEM;
}


static int 
myfs_get_super ( struct file_system_type * fs_type,
		 int flags, const  char *dev_name, void *data,
		 struct vfsmount *mnt)
{
  dbg("Getting Super Block");
  /* get_sb_single is called by vfs_kern_mount()
     we are using get_sb_single since its we are going to create a 
     fs without a block device.
 */
  return get_sb_single(fs_type, flags, data, myfs_fill_super, mnt);
}

static void 
myfs_kill_super ( struct super_block * super)
{
  kill_anon_super (super);
}

/* Setup the Module. */

static struct file_system_type myfs_type = {
  .owner = THIS_MODULE,
  .name = "myfs",
  .get_sb = myfs_get_super,
  .kill_sb = myfs_kill_super,
};

static  int myfs_init()
{
  int err = myfs_init_inodecache();
  if (err)
    goto out1;
 
  err = register_filesystem(&myfs_type);
  if (err) 
    goto out;
  return 0;
 out:
  myfs_destroy_inodecache();
 out1:
  return err;
}

static void myfs_exit()
{
  unregister_filesystem(&myfs_type);
  myfs_destroy_inodecache();
}


MODULE_LICENSE("GPL"); 		
MODULE_AUTHOR("Krishna G Pai"); 


module_init(myfs_init);
module_exit(myfs_exit);
