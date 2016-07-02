/*_############################################################################
  _## 
  _##  snmp_counters.h  
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

#ifndef snmp_counters_h_
#define snmp_counters_h_

#include <agent_pp/agent++.h>
#include <agent_pp/snmp_pp_ext.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

#define SNMP_COUNTERS 29



/**********************************************************************
 *  
 *  class MibIIsnmpCounter
 * 
 */

class AGENTPP_DECL MibIIsnmpCounters {

public:
	MibIIsnmpCounters();

	static NS_SNMP Counter32 inPkts()	       	{ return counter_snmp[0]; }
	static NS_SNMP Counter32 outPkts()	       	{ return counter_snmp[1]; }
	static NS_SNMP Counter32 inBadVersions()        { return counter_snmp[2]; }
	static NS_SNMP Counter32 inBadCommunityNames() 	{ return counter_snmp[3]; }
	static NS_SNMP Counter32 inBadCommunityUses()  	{ return counter_snmp[4]; }
	static NS_SNMP Counter32 inASNParseErrs()      	{ return counter_snmp[5]; }
	static NS_SNMP Counter32 inTooBigs()	       	{ return counter_snmp[6]; }
	static NS_SNMP Counter32 inNoSuchNames()       	{ return counter_snmp[7]; }
	static NS_SNMP Counter32 inBadValues()	       	{ return counter_snmp[8]; }
	static NS_SNMP Counter32 inReadOnlys()	       	{ return counter_snmp[9]; }
	static NS_SNMP Counter32 inGenErrs()	       	{ return counter_snmp[10]; }
	static NS_SNMP Counter32 inTotalReqVars()      	{ return counter_snmp[11]; }
	static NS_SNMP Counter32 inTotalSetVars()      	{ return counter_snmp[12]; }
	static NS_SNMP Counter32 inGetRequests()       	{ return counter_snmp[13]; }
	static NS_SNMP Counter32 inGetNexts()	       	{ return counter_snmp[14]; }
	static NS_SNMP Counter32 inSetRequests()       	{ return counter_snmp[15]; }
	static NS_SNMP Counter32 inGetResponses()      	{ return counter_snmp[16]; }
	static NS_SNMP Counter32 inTraps()	       	{ return counter_snmp[17]; }
	static NS_SNMP Counter32 outTooBigs()	       	{ return counter_snmp[18]; }
	static NS_SNMP Counter32 outNoSuchNames()      	{ return counter_snmp[19]; }
	static NS_SNMP Counter32 outBadValues()		{ return counter_snmp[20]; }
	static NS_SNMP Counter32 outGenErrs()	       	{ return counter_snmp[21]; }
	static NS_SNMP Counter32 outGetRequests()      	{ return counter_snmp[22]; }
	static NS_SNMP Counter32 outGetNexts()	       	{ return counter_snmp[23]; }
	static NS_SNMP Counter32 outSetRequests()      	{ return counter_snmp[24]; }
	static NS_SNMP Counter32 outGetResponses()     	{ return counter_snmp[25]; }
	static NS_SNMP Counter32 outTraps()	       	{ return counter_snmp[26]; }
	static NS_SNMP Counter32 silentDrops()      	{ return counter_snmp[27]; }
	static NS_SNMP Counter32 proxyDrops()     	{ return counter_snmp[28]; }


	static void      incInPkts()	       	{ counter_snmp[0]++; }
	static void      incOutPkts()	       	{ counter_snmp[1]++; }
	static void      incInBadVersions()     { counter_snmp[2]++; }
	static void      incInBadCommunityNames()     	{ counter_snmp[3]++; }
	static void      incInBadCommunityUses(){ counter_snmp[4]++; }
	static void      incInASNParseErrs()   	{ counter_snmp[5]++; }
	static void      incInTooBigs()	       	{ counter_snmp[6]++; }
	static void      incInNoSuchNames()    	{ counter_snmp[7]++; }
	static void      incInBadValues()       { counter_snmp[8]++; }
	static void      incInReadOnlys()	{ counter_snmp[9]++; }
	static void      incInGenErrs()	       	{ counter_snmp[10]++; }
	static void      incInTotalReqVars()   	{ counter_snmp[11]++; }
	static void      incInTotalSetVars()   	{ counter_snmp[12]++; }
	static void      incInGetRequests()    	{ counter_snmp[13]++; }
	static void      incInGetNexts()        { counter_snmp[14]++; }
	static void      incInSetRequests()    	{ counter_snmp[15]++; }
	static void      incInGetResponses()   	{ counter_snmp[16]++; }
	static void      incInTraps()	       	{ counter_snmp[17]++; }
	static void      incOutTooBigs()       	{ counter_snmp[18]++; }
	static void      incOutNoSuchNames()   	{ counter_snmp[19]++; }
	static void      incOutBadValues()      { counter_snmp[20]++; }
	static void      incOutGenErrs()        { counter_snmp[21]++; }
	static void      incOutGetRequests()   	{ counter_snmp[22]++; }
	static void      incOutGetNexts()       { counter_snmp[23]++; }
	static void      incOutSetRequests()   	{ counter_snmp[24]++; }
	static void      incOutGetResponses()  	{ counter_snmp[25]++; }
	static void      incOutTraps()	       	{ counter_snmp[26]++; }
	static void      incSilentDrops()  	{ counter_snmp[27]++; }
	static void      incProxyDrops()       	{ counter_snmp[28]++; }
	
	// set all counters to zero
	static void	 reset();

protected:
	static unsigned long counter_snmp[];
};

#ifdef AGENTPP_NAMESPACE
}
#endif
#endif
