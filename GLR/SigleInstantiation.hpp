/*
 * SigleInstantiation.hpp
 *
 *  Created on: Aug 29, 2013
 *      Author: aikon
 */

#ifndef SIGLEINSTANTIATION_HPP_
#define SIGLEINSTANTIATION_HPP_

#include <string>
#include <iostream>

#ifdef __cplusplus
extern "C"
{
#endif



#ifdef __cplusplus
}
#endif

class SigleInstantiation
{
public:
    SigleInstantiation(const char *const progname, const char *const pathname);
    ~SigleInstantiation();

private:
    int fd;
    std::string progname;
    std::string pathname;
};

int DaemonProcTerm(const char * const pathname);

#endif /* SIGLEINSTANTIATION_HPP_ */