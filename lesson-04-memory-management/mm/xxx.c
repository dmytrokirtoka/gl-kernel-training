#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/time64.h>
#include <linux/timekeeping.h>

#define LEN_MSG 160
static char buf_msg[ LEN_MSG + 1 ];
static time64_t prev_read_time = 0;

char* get_time( char* buf )
{
  struct timespec64 ts = current_kernel_time64();
  struct tm tm = {0};
  time64_t prt = prev_read_time;
  prev_read_time = ts.tv_sec;
  if (!prt)
  {
    sprintf( buf, "this is first read!\n" );
  }
  else
  {
    time64_to_tm( prt, 0, &tm );
    sprintf( buf, "%04ld/%02d/%02d %02d:%02d:%02d\n", tm.tm_year, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec );
  }
  return buf;
}


static ssize_t xxx_show( struct class *class, struct class_attribute *attr, char *buf ) {
   strcpy( buf, get_time( buf_msg ) );
   printk( "read %ld\n", (long)strlen( buf ) );
   return strlen( buf );
}

CLASS_ATTR_RO( xxx );

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
