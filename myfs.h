/**
   myfs.h 
*/

#ifndef _MYFS_H_
#define _MYFS_H_

#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/statfs.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include <asm/errno.h>
#include <linux/buffer_head.h>
#include <linux/dcache.h>


#define MYFS_MAGIC 0x19830507

#if !defined (CONFIG_MYFS_MODULE)
#define MY_NAME "myfs"
#else 
#define MY_NAME THIS_MODULE
#endif 

#undef dbg
#ifdef MYFS_DEBUG
# ifdef __KERNEL__
  #define dbg(fmt, args...)   printk( KERN_DEBUG MY_NAME ": "  fmt "\n", ## args)      
# endif
#else 
#define dbg(fmt, args...) 
#endif

extern struct inode * myfs_iget(struct super_block * , unsigned long );
extern void myfs_set_inode ( struct inode *, dev_t );
extern struct inode * myfs_new_inode(const struct inode *, int *);
extern ssize_t myfs_write_data(struct inode * inode, void * data, size_t count,
			 loff_t * offset);
extern void myfs_truncate(struct inode * );
extern int myfs_getattr(struct vfsmount*, struct dentry *, struct kstat *);
extern int myfs_add_link(struct dentry *, struct inode * );
extern ino_t myfs_inode_by_name(struct dentry * dentry);
extern int myfs_make_empty(struct inode * inode, struct inode * dir);

#define MYFS_DIRECT_BLOCKS  10
#define MYFS_L1_INDIRECT_BLOCKS 10
#define MYFS_INODE_QUEUE  100

struct myfs_inode {
        struct inode            vfs_inode;
        void *                  mi_data_blocks[MYFS_DIRECT_BLOCKS];
};

static inline struct myfs_inode *myfs_i(struct inode *inode)
{
	return list_entry(inode, struct myfs_inode, vfs_inode);
}

struct myfs_dentry{
        unsigned long       inode;
        unsigned int        len;
        char                valid; /* If Non zero , means invalid dirent */
        char                name[55]; 
}; 				/* Optimized for 64 bytes. */

struct myfs_sbinfo{
        unsigned long     *numarr;
        unsigned int       len;
        unsigned int       cur;
        unsigned long      maxnum;
};


extern const struct inode_operations myfs_dir_inode_operations ;
extern const struct file_operations myfs_file_operations;
extern const struct inode_operations myfs_file_inode_operations;
extern const struct file_operations myfs_dir_operations;

#endif 
