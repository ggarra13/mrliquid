/**
 * @file   mrvProtection.cpp
 * @author gga
 * @date   Mon Oct 30 04:55:39 2006
 * 
 * @brief  Functions used to read and check license file
 * 
 */


#include "mrIO.h"
#include "mrlLicensing.h"
// #include "mrvPC1.h"

extern "C" {
  // RLM files
  //#include "license.h"
  //#include "license_to_run.h"
}

namespace mrl
{
  // namespace {

  //   //
  //   // 128 SHA-1 encode digest of 'gga_mrLiquid_key'
  //   //
  //   // Key to PC1 algorithm
  //   //
  //   const char* kPC1_key = 
  //     "O\317\355\2335Uo)d\312N8T\221\373\243g\024\213:";

  //   // "2764@127.0.0.1";
  //   const char* kDefaultServer = "fnlgdpnljhbibnidkkekenlbklif"; 

  //   // "Error initializing license system:\n"
  //   const char* kLicenseInitError = "ckplppmjmhpjnobikigbjbfaidkfbamgkkakaiejjcfcaaaijkkeejfeinijgppodlneha";

  //   // "License error:\n"
  //   const char* kLicenseError = "cdojnmnpnkahognhopdomhjpokbigc";

  //   // "mrLiquid"
  //   const char* kProduct = "acchjjfnjhapkeek";

  //   // "2007.01"
  //   const char* kVersion = "fnlbfdcmnefjgg";


  //   RLM_HANDLE   rh = NULL;
  //   RLM_LICENSE lic = NULL;

  //   mrv::cipher::PC1 cypher;

  // }


  // char* decrypt( const char* text )
  // {
  //   cypher.ascipherlen = strlen(text);
  //   cypher.plainlen    = strlen(text);
  //   cypher.ascii_decrypt128( text, kPC1_key );
  //   return cypher.plainText;
  // }


  bool open_license()
  {
    // char* product = decrypt( kProduct );
    // char* server  = decrypt( kDefaultServer );

    // rh = rlm_init( ".", product, server );

    // free( product );
    // free( server );

    // int stat = rlm_stat(rh);
    // if ( stat )
    //   {
    // 	char errstring[RLM_ERRSTRING_MAX];

    // 	LOG_ERROR( decrypt( kLicenseInitError ) );
    // 	LOG_ERROR( rlm_errstring( (RLM_LICENSE) NULL, rh, errstring ) );
    // 	close_license();
    // 	return false;
    //   }

    return true;
  }

  void printstat()
  {
    // char errstring[RLM_ERRSTRING_MAX];

    // LOG_ERROR( decrypt( kLicenseError ) );
    // LOG_ERROR( rlm_errstring(lic, rh, errstring) );

    checkin_license();
    close_license();

    // OUCH!  We should not exit, should we?
    // exit(1);
  }

  bool checkout_license()
  {
    // if ( !rh ) 
    //   {
    // 	if ( ! open_license() ) return false;
    //   }

    // char* product = decrypt( kProduct );
    // char* version = decrypt( kVersion );

    // lic = rlm_checkout(rh, product, version, 1);

    // free( product );
    // free( version );

    // if ( !lic ) return false;
    return true;
  }

  bool check_license_status()
  {
    // if ( !lic )
    //   {
    // 	if ( !checkout_license() ) return false;
    //   }

    // int stat = rlm_license_stat(lic);
    // if ( stat == 0 || stat == RLM_EL_INQUEUE || stat == RLM_EL_OVERSOFT) 
    
     return true;

    // printstat();
    // return false;
  }

  void checkin_license()
  {
    // if (lic)  rlm_checkin(lic);
    // lic = NULL;
  }


  void close_license()
  {
    // checkin_license();
    // rlm_close(rh);
    // rh = NULL;
  }

}
