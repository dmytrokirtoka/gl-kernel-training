#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/math64.h>

#define LEN_MSG 160
static char buf_msg[ LEN_MSG + 1 ];
#define FMTSTR "last time is %s\n"
static uint64_t last_time = 0;

static char* print_time( char* buf )
{
  char numstr[32] = {0};
  if (last_time == 0)
    strcpy( numstr, "first time" );
  else
  {
    uint64_t lt = last_time;
    last_time = get_jiffies_64();
    sprintf( numstr, "%llu sec", div64_u64(last_time - lt , HZ ) );
  }
  sprintf( buf, FMTSTR, numstr );
  return buf;
}

static ssize_t xxx_show( struct class *class, struct class_attribute *attr, char *buf ) {
   
  strcpy( buf, print_time(buf_msg) );
  printk( "read %ld\n", (long)strlen( buf ) );
  return strlen( buf );
}

static ssize_t xxx_store( struct class *class, struct class_attribute *attr, const char *buf, size_t count ) {
   printk( "write %ld\n", (long)count );
   //reset interval count
   last_time = 0;
   return count;
}

CLASS_ATTR_RW( xxx );

static struct class *x_class;

int __init x_init(void) {
   int res;
   x_class = class_create( THIS_MODULE, "x-class" );
   if( IS_ERR( x_class ) ) printk( "bad class create\n" );
   res = class_create_file( x_class, &class_attr_xxx );
   printk( "'xxx' module initialized\n" );
   return res;
}

void x_cleanup(void) {
   class_remove_file( x_class, &class_attr_xxx );
   class_destroy( x_class );
   return;
}

module_init( x_init );
module_exit( x_cleanup );

MODULE_LICENSE( "GPL" );
