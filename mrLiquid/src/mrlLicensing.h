/**
 * @file   mrLicense.h
 * @author gga
 * @date   Sat Oct 28 05:53:19 2006
 * 
 * @brief  A simple copy-protection/license mechanism
 * 
 * 
 */
#ifndef mrlLicensing_h
#define mrlLicensing_h

#include <ctime>
#include <string>

namespace mrl
{

  bool open_license();
  void close_license();

  bool checkout_license();
  bool check_license_status();
  void checkin_license();

}

#endif // mrlLicensing
