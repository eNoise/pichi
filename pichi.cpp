#include <iostream>
#include <embed/php_embed.h>
#include <zend_extensions.h>

int main(int argc, char* argv[])
{
    php_embed_init(argc, argv PTSRMLS_CC);
    
    zval ret;
    int exit_status;

    //settings
    //std::cout << zend_startup_extensions();
    //--
    
    zend_first_try
    {
        if(zend_eval_string("include('main.php');", &ret, "main" TSRMLS_CC) == FAILURE)
    	    std::cout << "Syntaxis error\nSend mail to delevelopers deg@uruchie.org\nend all must be ok ;)" << std::endl;
    	    
    	exit_status = Z_LVAL(ret);
    }
    zend_catch
    {
	exit_status = EG(exit_status);
    }
    zend_end_try();
    
    php_embed_shutdown(TSRMLS_C);
    return exit_status;
}
