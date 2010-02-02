#include <stdio.h>

struct myfs_dentry{
        unsigned long       inode;
        unsigned int        len;
        char                name[52]; 
};

int main()
{

  printf("sizeof myfs_dentry:%d\n",sizeof(struct myfs_dentry));
  printf("sizeof unsigned long:%d\n",sizeof(unsigned long));
  printf("sizeof unsigned int:%d\n",sizeof(unsigned int));
}
