/*_############################################################################
  _## 
  _##  snmp_community_mib.h  
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


#ifndef _snmp_community_mib_h
#define _snmp_community_mib_h


#include <agent_pp/mib.h>
#include <agent_pp/snmp_textual_conventions.h>
#include <agent_pp/notification_originator.h>
#include <agent_pp/snmp_target_mib.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

#define oidSnmpCommunityTable            "1.3.6.1.6.3.18.1.1"
#define oidSnmpCommunityEntry            "1.3.6.1.6.3.18.1.1.1"
#define oidSnmpCommunityIndex            "1.3.6.1.6.3.18.1.1.1.1"
#define oidSnmpCommunityName             "1.3.6.1.6.3.18.1.1.1.2"
#define oidSnmpCommunitySecurityName     "1.3.6.1.6.3.18.1.1.1.3"
#define oidSnmpCommunityContextEngineID  "1.3.6.1.6.3.18.1.1.1.4"
#define oidSnmpCommunityContextName      "1.3.6.1.6.3.18.1.1.1.5"
#define oidSnmpCommunityTransportTag     "1.3.6.1.6.3.18.1.1.1.6"
#define oidSnmpCommunityStorageType      "1.3.6.1.6.3.18.1.1.1.7"
#define oidSnmpCommunityStatus           "1.3.6.1.6.3.18.1.1.1.8"
#define oidSnmpTargetAddrExtTable        "1.3.6.1.6.3.18.1.2"
#define oidSnmpTargetAddrExtEntry        "1.3.6.1.6.3.18.1.2.1"
#define oidSnmpTargetAddrTMask           "1.3.6.1.6.3.18.1.2.1.1"
#define oidSnmpTargetAddrMMS             "1.3.6.1.6.3.18.1.2.1.2"
#define oidSnmpTrapAddress               "1.3.6.1.6.3.18.1.3.0"
#define oidSnmpTrapCommunity             "1.3.6.1.6.3.18.1.4.0"

#ifdef _SNMPv3

/**
 *  snmpCommunityName
 *
"The community string for which a row in this table
 represents a configuration."
 */


/**
 *  snmpCommunitySecurityName
 *
"A human readable string representing the corresponding
 value of snmpCommunityName in a Security Model
 independent format."
 */


/**
 *  snmpCommunityContextEngineID
 *
"The contextEngineID indicating the location of the
 context in which management information is accessed
 when using the community string specified by the
 corresponding instance of snmpCommunityName.

 The default value is the snmpEngineID of the entity in
 which this object is instantiated."
 */


/**
 *  snmpCommunityContextName
 *
"The context in which management information is accessed
 when using the community string specified by the corresponding
 instance of snmpCommunityName."
 */



/**
 *  snmpCommunityTransportTag
 *
"This object specifies a set of transport endpoints
 from which a command responder application will accept
 management requests. If a management request containing
 this community is received on a transport endpoint other
 than the transport endpoints identified by this object,
 the request is deemed unauthentic.

 The transports identified by this object are specified
 in the snmpTargetAddrTable. Entries in that table
 whose snmpTargetAddrTagList contains this tag value
 are identified.

 If the value of this object has zero-length, transport
 endpoints are not checked when authenticating messages
 containing this community string."
 */


/**
 *  snmpCommunityStorageType
 *
"The storage type for this conceptual row in the
 snmpCommunityTable. Conceptual rows having the value
 'permanent' need not allow write-access to any
 columnar object in the row."
 */


/**
 *  snmpCommunityStatus
 *
"The status of this conceptual row in the snmpCommunityTable.

 An entry in this table is not qualified for activation
 until instances of all corresponding columns have been
 initialized, either through default values, or through
 Set operations. The snmpCommunityName and
 snmpCommunitySecurityName objects must be explicitly set.

 There is no restriction on setting columns in this table
 when the value of snmpCommunityStatus is active(1)."
 */


/**
 *  snmpTargetAddrTMask
 *
"The mask value associated with an entry in the
 snmpTargetAddrTable. The value of this object must
 have the same length as the corresponding instance of
 snmpTargetAddrTAddress, or must have length 0. An
 attempt to set it to any other value will result in
 an inconsistentValue error.

 The value of this object allows an entry in the
 snmpTargetAddrTable to specify multiple addresses.
 The mask value is used to select which bits of
 a transport address must match bits of the corresponding
 instance of snmpTargetAddrTAddress, in order for the
 transport address to match a particular entry in the
 snmpTargetAddrTable. Bits which are 1 in the mask
 value indicate bits in the transport address which
 must match bits in the snmpTargetAddrTAddress value.
 Bits which are 0 in the mask indicate bits in the
 transport address which need not match. If the
 length of the mask is 0, the mask should be treated
 as if all its bits were 1 and its length were equal
 to the length of the corresponding value of
 snmpTargetAddrTable.

 This object may not be modified while the value of the
 corresponding instance of snmpTargetAddrRowStatus is
 active(1). An attempt to set this object in this case
 will result in an inconsistentValue error."
 */


class AGENTPP_DECL snmpTargetAddrTMask: public snmpTargetAddrTAddress {

public:
	snmpTargetAddrTMask(const Oidx&);
	virtual ~snmpTargetAddrTMask();

	virtual MibEntryPtr	clone();
	virtual int        	prepare_set_request(Request*, int&);
	virtual NS_SNMP UdpAddress*	getUdpAddress();
};


/**
 *  snmpTargetAddrMMS
 *
"The maximum message size value associated with an entry
 in the snmpTargetAddrTable."
 */


/**
 *  snmpCommunityEntry
 *
"Information about a particular community string."
 */


class AGENTPP_DECL snmpCommunityEntry: public StorageTable {

public:
	snmpCommunityEntry();
	virtual ~snmpCommunityEntry();

	static snmpCommunityEntry* instance;

	virtual void       	set_row(MibTableRow* r, const NS_SNMP OctetStr& p0, 
					const NS_SNMP OctetStr& p1, 
					const NS_SNMP OctetStr& p2, 
					const NS_SNMP OctetStr& p3, 
					const NS_SNMP OctetStr& p4, int p5, 
					int p6);
	/**
	 * Get the v3 information for a community.
	 *
	 * @param securityName
	 *    in:  the community name to lookup.
	 *    out: the corresponding securityName.
	 * @param contextEngineId
	 *    in:  ignored.
	 *    out: the contextEngineId for the given community.
	 * @param contextName
	 *    in:  ignored.
	 *    out: the contextName associated with the given community.
	 * @param transportTag
	 *    in:  ignored.
	 *    out: the transportTag associated with the given community.
	 * @return
	 *    TRUE if an entry for the given community could be found,
	 *    FALSE otherwise.
	 */
	virtual boolean		get_v3_info(NS_SNMP OctetStr&, NS_SNMP OctetStr&,
					    NS_SNMP OctetStr&, NS_SNMP OctetStr&);

	/**
	 * Get the community for a given securityName/contextEngineId/ 
	 * contextName combination.
	 *
	 * @param securityName
	 *    a security name. On return it contains the found community.
	 * @param contextEngineId
	 *    a context engine ID.
	 * @param contextName
	 *    a context name.
	 * @return
	 *    TRUE if an appropriate community could be found,
	 *    FALSE otherwise. 
	 */
	virtual boolean		get_community(NS_SNMP OctetStr&, 
					      const NS_SNMP OctetStr&, 
					      const NS_SNMP OctetStr&);
};


/**
 *  snmpTargetAddrExtEntry
 *
"Information about a particular mask and mms value."
 */


class AGENTPP_DECL snmpTargetAddrExtEntry: public MibTable {

public:
	snmpTargetAddrExtEntry();
	virtual ~snmpTargetAddrExtEntry();

	static snmpTargetAddrExtEntry* instance;

	virtual void        	row_added(MibTableRow*, const Oidx&, 
					  MibTable* s = 0);
	virtual void        	row_delete(MibTableRow*, const Oidx&, 
					   MibTable* s = 0);
	virtual void       	set_row(MibTableRow* r, const NS_SNMP OctetStr& p0, 
					int p1);
	virtual int        	prepare_set_request(Request*, int&);
#ifdef _SNMPv3
	virtual boolean		passes_filter(const NS_SNMP OctetStr&, const NS_SNMP UTarget&);
#endif
	virtual boolean		passes_filter(const NS_SNMP OctetStr&,const NS_SNMP OctetStr&);
};


class AGENTPP_DECL snmp_community_mib: public MibGroup
{
  public:
	snmp_community_mib();
	virtual ~snmp_community_mib() { }
	static void	       	add_public();
};
#endif

#ifdef AGENTPP_NAMESPACE
}
#endif

/**
 * snmp_community_mib.h generated by AgentGen 1.1.4 for AGENT++v3 
 * Mon Nov 15 23:09:36 GMT+03:30 1999.
 */


#endif


