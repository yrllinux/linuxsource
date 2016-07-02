/*_############################################################################
  _## 
  _##  snmp_group.h  
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


#ifndef snmp_group_h_
#define snmp_group_h_


#include <string.h>

#include <agent_pp/agent++.h>
#include <agent_pp/mib.h>


#define oidSnmpGroup			"1.3.6.1.2.1.11"
#define oidSnmpInPkts			"1.3.6.1.2.1.11.1.0"
#define oidSnmpOutPkts			"1.3.6.1.2.1.11.2.0"
#define oidSnmpInBadVersions		"1.3.6.1.2.1.11.3.0"
#define oidSnmpInBadCommunityNames 	"1.3.6.1.2.1.11.4.0"
#define oidSnmpInBadCommunityUses  	"1.3.6.1.2.1.11.5.0"
#define oidSnmpInASNParseErrs      	"1.3.6.1.2.1.11.6.0"
#define oidSnmpInTooBigs	       	"1.3.6.1.2.1.11.8.0"
#define oidSnmpInNoSuchNames       	"1.3.6.1.2.1.11.9.0"
#define oidSnmpInBadValues	       	"1.3.6.1.2.1.11.10.0"
#define oidSnmpInReadOnlys	       	"1.3.6.1.2.1.11.11.0"
#define oidSnmpInGenErrs	       	"1.3.6.1.2.1.11.12.0"
#define oidSnmpInTotalReqVars      	"1.3.6.1.2.1.11.13.0"
#define oidSnmpInTotalSetVars      	"1.3.6.1.2.1.11.14.0"
#define oidSnmpInGetRequests       	"1.3.6.1.2.1.11.15.0"
#define oidSnmpInGetNexts	       	"1.3.6.1.2.1.11.16.0"
#define oidSnmpInSetRequests       	"1.3.6.1.2.1.11.17.0"
#define oidSnmpInGetResponses      	"1.3.6.1.2.1.11.18.0"
#define oidSnmpInTraps	       	        "1.3.6.1.2.1.11.19.0"
#define oidSnmpOutTooBigs	       	"1.3.6.1.2.1.11.20.0"
#define oidSnmpOutNoSuchNames      	"1.3.6.1.2.1.11.21.0"
#define oidSnmpOutBadValues		"1.3.6.1.2.1.11.22.0"
#define oidSnmpOutGenErrs	       	"1.3.6.1.2.1.11.24.0"
#define oidSnmpOutGetRequests      	"1.3.6.1.2.1.11.25.0"
#define oidSnmpOutGetNexts	       	"1.3.6.1.2.1.11.26.0"
#define oidSnmpOutSetRequests      	"1.3.6.1.2.1.11.27.0"
#define oidSnmpOutGetResponses     	"1.3.6.1.2.1.11.28.0"
#define oidSnmpOutTraps			"1.3.6.1.2.1.11.29.0"
#define oidSnmpEnableAuthenTraps	"1.3.6.1.2.1.11.30.0"
#define oidSnmpSilentDrops	       	"1.3.6.1.2.1.11.31.0"
#define oidSnmpProxyDrops		"1.3.6.1.2.1.11.32.0"

#define enableAuthTraps			1
#define disableAuthTraps	        2

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


class AGENTPP_DECL snmpInPkts: public MibLeaf {

public:
	snmpInPkts(): MibLeaf(oidSnmpInPkts, READONLY, new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutPkts: public MibLeaf {

public:
	snmpOutPkts(): MibLeaf(oidSnmpOutPkts, READONLY, new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInBadVersions: public MibLeaf {

public:
	snmpInBadVersions(): MibLeaf(oidSnmpInBadVersions, READONLY, 
			       new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInBadCommunityNames: public MibLeaf {

public:
	snmpInBadCommunityNames(): MibLeaf(oidSnmpInBadCommunityNames, 
					   READONLY, 
					   new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInBadCommunityUses: public MibLeaf {

public:
	snmpInBadCommunityUses(): MibLeaf(oidSnmpInBadCommunityUses, 
					  READONLY, 
					  new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInASNParseErrs: public MibLeaf {

public:
	snmpInASNParseErrs(): MibLeaf(oidSnmpInASNParseErrs, 
				      READONLY, 
				      new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInTooBigs: public MibLeaf {

public:
	snmpInTooBigs(): MibLeaf(oidSnmpInTooBigs, 
				 READONLY, 
				 new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInNoSuchNames: public MibLeaf {

public:
	snmpInNoSuchNames(): MibLeaf(oidSnmpInNoSuchNames, 
				     READONLY, 
				     new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInBadValues: public MibLeaf {

public:
	snmpInBadValues(): MibLeaf(oidSnmpInBadValues, 
				   READONLY, 
				   new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInReadOnlys: public MibLeaf {

public:
	snmpInReadOnlys(): MibLeaf(oidSnmpInReadOnlys, 
				   READONLY, 
				   new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInGenErrs: public MibLeaf {

public:
	snmpInGenErrs(): MibLeaf(oidSnmpInGenErrs, 
				 READONLY, 
				 new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInTotalReqVars: public MibLeaf {

public:
	snmpInTotalReqVars(): MibLeaf(oidSnmpInTotalReqVars, 
				      READONLY, 
				      new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInTotalSetVars: public MibLeaf {

public:
	snmpInTotalSetVars(): MibLeaf(oidSnmpInTotalSetVars, 
				      READONLY, 
				      new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInGetRequests: public MibLeaf {

public:
	snmpInGetRequests(): MibLeaf(oidSnmpInGetRequests, 
				     READONLY, 
				     new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInGetNexts: public MibLeaf {

public:
	snmpInGetNexts(): MibLeaf(oidSnmpInGetNexts, 
				  READONLY, 
				  new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInSetRequests: public MibLeaf {

public:
	snmpInSetRequests(): MibLeaf(oidSnmpInSetRequests, 
				     READONLY, 
				     new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInGetResponses: public MibLeaf {

public:
	snmpInGetResponses(): MibLeaf(oidSnmpInGetResponses, 
				      READONLY, 
				      new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpInTraps: public MibLeaf {

public:
	snmpInTraps(): MibLeaf(oidSnmpInTraps, 
			       READONLY, 
			       new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutTooBigs: public MibLeaf {

public:
	snmpOutTooBigs(): MibLeaf(oidSnmpOutTooBigs, 
				  READONLY, 
				  new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutNoSuchNames: public MibLeaf {

public:
	snmpOutNoSuchNames(): MibLeaf(oidSnmpOutNoSuchNames, 
				      READONLY, 
				      new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutBadValues: public MibLeaf {

public:
	snmpOutBadValues(): MibLeaf(oidSnmpOutBadValues, 
				    READONLY, 
				    new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutGenErrs: public MibLeaf {

public:
	snmpOutGenErrs(): MibLeaf(oidSnmpOutGenErrs, 
				  READONLY, 
				  new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutGetRequests: public MibLeaf {

public:
	snmpOutGetRequests(): MibLeaf(oidSnmpOutGetRequests, 
				      READONLY, 
				      new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutGetNexts: public MibLeaf {

public:
	snmpOutGetNexts(): MibLeaf(oidSnmpOutGetNexts, 
				   READONLY, 
				   new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutSetRequests: public MibLeaf {

public:
	snmpOutSetRequests(): MibLeaf(oidSnmpOutSetRequests, 
				      READONLY, 
				      new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutGetResponses: public MibLeaf {

public:
	snmpOutGetResponses(): MibLeaf(oidSnmpOutGetResponses, 
				       READONLY, 
				       new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpOutTraps: public MibLeaf {

public:
	snmpOutTraps(): MibLeaf(oidSnmpOutTraps, 
				READONLY, 
				new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};

class AGENTPP_DECL snmpSilentDrops: public MibLeaf {

public:
	snmpSilentDrops(): MibLeaf(oidSnmpSilentDrops,
				   READONLY, 
				   new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


class AGENTPP_DECL snmpProxyDrops: public MibLeaf {

public:
	snmpProxyDrops(): MibLeaf(oidSnmpProxyDrops,
				  READONLY, 
				  new NS_SNMP Counter32(0)) { }
	
	virtual void get_request(Request*, int); 
	virtual boolean is_volatile() { return TRUE; }
};


/**
 *  snmpEnableAuthenTraps
 *
"Indicates whether the SNMP entity is permitted to generate
 authenticationFailure traps. The value of this object
 overrides any configuration information; as such, it
 provides a means whereby all authenticationFailure traps may
 be disabled.

 Note that it is strongly recommended that this object be
 stored in non-volatile memory so that it remains constant
 across re-initializations of the network management system."
 */


class AGENTPP_DECL snmpEnableAuthenTraps: public MibLeaf {

public:
	snmpEnableAuthenTraps();
	virtual ~snmpEnableAuthenTraps();

	static snmpEnableAuthenTraps* instance;
	long			get_state();
	virtual boolean    	value_ok(const Vbx&);
};


/**********************************************************************
 * 
 *  class snmpGroup
 *
 **********************************************************************/


class AGENTPP_DECL snmpGroup: public MibGroup {

public:
	snmpGroup();
};
#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
