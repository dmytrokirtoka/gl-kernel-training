#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/timer.h>

static int n1, n2;
static int n[1];
static void fib_handler( unsigned long );
DEFINE_TIMER( mytimer, fib_handler, 0, 0 );

void fib_handler( unsigned long data )
{
  if (*n)
  {
    int t = n2;
    n2 = n1 + n2;  
    if (n2 < 0)
    {
      n1 = 0; n2 = 1;
    }
    else  
      n1 = t;

    *n = n2; //atomic
  }
  else
  {
    n1 = 0; n2 = 1;
  }
  
  mod_timer( &mytimer, jiffies + HZ );
}

static ssize_t xxx_show( struct class *class, struct class_attribute *attr, char *buf ) {
   sprintf( buf, "fibonacci: %d\n", *n );
   printk( "read %ld\n", (long)strlen( buf ) );
   return strlen( buf );
}

static ssize_t xxx_store( struct class *class, struct class_attribute *attr, const char *buf, size_t count ) {
   printk( "reset fibonacci\n" );
   *n = 0;
   return count;
}

CLASS_ATTR_RW( xxx );

static struct class *x_class;

int __init x_init(void) {
   int res;
   x_class = class_create( THIS_MODULE, "x-class" );
   if( IS_ERR( x_class ) ) printk( "bad class create\n" );
   res = class_create_file( x_class, &class_attr_xxx );
   mod_timer( &mytimer, jiffies + HZ );
   printk( "'xxx' module initialized\n" );
   return res;
}

void x_cleanup(void) {
   del_timer( &mytimer );
   class_remove_file( x_class, &class_attr_xxx );
   class_destroy( x_class );
   return;
}

module_init( x_init );
module_exit( x_cleanup );

MODULE_LICENSE( "GPL" );
