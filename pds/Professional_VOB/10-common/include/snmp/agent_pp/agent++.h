/*_############################################################################
  _## 
  _##  agent++.h  
  _## 
  _##
  _##  AGENT++ API Version 3.5.23a
  _##  -----------------------------------------------
  _##  Copyright (C) 2000-2005 Frank Fock, Jochen Katz
  _##  
  _##  LICENSE AGREEMENT
  _##
  _##  WHEREAS,  Frank  Fock  and  Jochen  Katz  are  the  owners of valuable
  _##  intellectual  property rights relating to  the AGENT++ API and wish to
  _##  license AGENT++ subject to the  terms and conditions set forth  below;
  _##  and
  _##
  _##  WHEREAS, you ("Licensee") acknowledge  that Frank Fock and Jochen Katz
  _##  have the right  to grant licenses  to the intellectual property rights
  _##  relating to  AGENT++, and that you desire  to obtain a license  to use
  _##  AGENT++ subject to the terms and conditions set forth below;
  _##
  _##  Frank  Fock    and Jochen   Katz   grants  Licensee  a  non-exclusive,
  _##  non-transferable, royalty-free  license  to use   AGENT++ and  related
  _##  materials without  charge provided the Licensee  adheres to all of the
  _##  terms and conditions of this Agreement.
  _##
  _##  By downloading, using, or  copying  AGENT++  or any  portion  thereof,
  _##  Licensee  agrees to abide  by  the intellectual property  laws and all
  _##  other   applicable laws  of  Germany,  and  to all of   the  terms and
  _##  conditions  of this Agreement, and agrees  to take all necessary steps
  _##  to  ensure that the  terms and  conditions of  this Agreement are  not
  _##  violated  by any person  or entity under the  Licensee's control or in
  _##  the Licensee's service.
  _##
  _##  Licensee shall maintain  the  copyright and trademark  notices  on the
  _##  materials  within or otherwise  related   to AGENT++, and  not  alter,
  _##  erase, deface or overprint any such notice.
  _##
  _##  Except  as specifically   provided in  this  Agreement,   Licensee  is
  _##  expressly   prohibited  from  copying,   merging,  selling,   leasing,
  _##  assigning,  or  transferring  in  any manner,  AGENT++ or  any portion
  _##  thereof.
  _##
  _##  Licensee may copy materials   within or otherwise related   to AGENT++
  _##  that bear the author's copyright only  as required for backup purposes
  _##  or for use solely by the Licensee.
  _##
  _##  Licensee may  not distribute  in any  form  of electronic  or  printed
  _##  communication the  materials  within or  otherwise  related to AGENT++
  _##  that  bear the author's  copyright, including  but  not limited to the
  _##  source   code, documentation,  help  files, examples,  and benchmarks,
  _##  without prior written consent from the authors.  Send any requests for
  _##  limited distribution rights to fock@agentpp.com.
  _##
  _##  Licensee  hereby  grants  a  royalty-free  license  to  any  and   all 
  _##  derivatives  based  upon this software  code base,  that  may  be used
  _##  as a SNMP  agent development  environment or a  SNMP agent development 
  _##  tool.
  _##
  _##  Licensee may  modify  the sources  of AGENT++ for  the Licensee's  own
  _##  purposes.  Thus, Licensee  may  not  distribute  modified  sources  of
  _##  AGENT++ without prior written consent from the authors. 
  _##
  _##  The Licensee may distribute  binaries derived from or contained within
  _##  AGENT++ provided that:
  _##
  _##  1) The Binaries are  not integrated,  bundled,  combined, or otherwise
  _##     associated with a SNMP agent development environment or  SNMP agent
  _##     development tool; and
  _##
  _##  2) The Binaries are not a documented part of any distribution material. 
  _##
  _##
  _##  THIS  SOFTWARE  IS  PROVIDED ``AS  IS''  AND  ANY  EXPRESS OR  IMPLIED
  _##  WARRANTIES, INCLUDING, BUT NOT LIMITED  TO, THE IMPLIED WARRANTIES  OF
  _##  MERCHANTABILITY AND FITNESS FOR  A PARTICULAR PURPOSE  ARE DISCLAIMED.
  _##  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
  _##  INDIRECT,   INCIDENTAL,  SPECIAL, EXEMPLARY,  OR CONSEQUENTIAL DAMAGES
  _##  (INCLUDING,  BUT NOT LIMITED  TO,  PROCUREMENT OF SUBSTITUTE  GOODS OR
  _##  SERVICES; LOSS OF  USE,  DATA, OR PROFITS; OR  BUSINESS  INTERRUPTION)
  _##  HOWEVER CAUSED  AND ON ANY THEORY  OF  LIABILITY, WHETHER IN CONTRACT,
  _##  STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
  _##  IN  ANY WAY OUT OF  THE USE OF THIS  SOFTWARE,  EVEN IF ADVISED OF THE
  _##  POSSIBILITY OF SUCH DAMAGE. 
  _##
  _##
  _##  Stuttgart, Germany, Sat Jun 18 13:06:20 CEST 2005 
  _##  
  _##########################################################################*/

#ifndef _agentpp_h_
#define _agentpp_h_

#define AGENTPP_VERSION_STRING "3.5.23"
#define AGENTPP_VERSION 3
#define AGENTPP_RELEASE 5
#define AGENTPP_PATCHLEVEL 23

// system dependend witches
#ifndef NO_NANOSLEEP
#define nanosleep nanosleep
#endif

// Agent++ configuration options
//
// (Note that Agent++ and SNMP++ should be built with the same settings!)

// define _NO_LOGGING in snmp++/include/config_snmp_pp.h if you do not
// want any logging output (increases performance drastically and
// minimizes memory consumption)

// define _NO_THREADS in snmp++/include/config_snmp_pp.h if you do not
// want thread support

// define _NO_SNMPv3 in snmp++/include/config_snmp_pp.h if you do not
// want v3 support

// snmp_pp.h includes config_snmp_pp.h, which checks whether _SNMPv3 and
// thread support are defined
#include <snmp_pp/snmp_pp.h>


// define _PROXY_FORWARDER for v1/v2c/v3 proxy agents
#define _PROXY_FORWARDER

#include <snmp_pp/smi.h>

#define	GENERIC_CLASS_SUCCESS		0
#define	GENERIC_CLASS_ERROR		1

#define GENERIC_CLASS_TYPE_MISMATCH	-1

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef boolean
#define boolean int
#endif

// internal "snmp" pdu types, used for internal mib updates
// update: update one or more managed objects
//         NOTE: if the target object is a table object and
//               if the target object does not exist an new row is created
// remove: remove one or more managed objects
//         NOTE: if the target object is a table object 
//               the complete row is removed

#define MIB_UPDATE		(aSN_UNIVERSAL | aSN_CONSTRUCTOR | 0x0A )
#define MIB_REMOVE	       	(aSN_UNIVERSAL | aSN_CONSTRUCTOR | 0x0B )

typedef enum { READING, WRITING } access_types;

#ifndef AGENTPP_DECL
	#if defined (WIN32) && defined (AGENT_PP_DLL)
		#ifdef AGENT_PP_EXPORTS
			#define AGENTPP_DECL __declspec(dllexport)
			#define AGENTPP_DECL_TEMPL
			#pragma warning (disable : 4018)	// signed/unsigned mismatch when exporting templates
		#else
			#define AGENTPP_DECL __declspec(dllimport)
			#define AGENTPP_DECL_TEMPL extern
			#pragma warning (disable : 4231)	// disable warnings on extern before template instantiation
		#endif
	#else
		#define AGENTPP_DECL
// Seems that a eplicit extern declaration is needed on some 
// systems. If you encounter linking problems you may uncomment
// the following:
//#ifdef WIN32 
		#define AGENTPP_DECL_TEMPL
//#else
//		#define AGENTPP_DECL_TEMPL extern
//#endif
	#endif
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock.h>
#include <time.h>
#include <process.h>
#endif

// Options about program code alternatives

// undef this if you want to use OrderedList instead OrderedArray
#define USE_ARRAY_TEMPLATE

// Define STATIC_REQUEST_LIST if you need a static RequestList in
// Mib.
//#define STATIC_REQUEST_LIST

// ThreadPool may not work on any system so disable it if you encounter
// problems
#ifdef _THREADS
#define AGENTPP_USE_THREAD_POOL

// Use NO_FAST_MUTEXES on systems that do not allow a thread to unlock
// a mutex it does not own. This has to be defined on WIN32 systems!
#ifdef WIN32
#define NO_FAST_MUTEXES
#endif
#endif //_THREADS

// SnmpRequest and SnmpRequestV3 use temporary Snmpx objects for sending
// traps, informs and requests. The default is to create these objects
// with listen address 0.0.0.0. If you define this, they are created
// using the listen address of the RequestList.
#define USE_LISTEN_ADDRESS_FOR_SENDING_TRAPS


#ifdef __GNUC__
#if (__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 91))
#define AGENTPP_NAMESPACE
#endif
#endif

#define DEFAULT_ENGINE_BOOTS_FILE "snmpv3_boot_counter"

#endif // _agentpp_h_
