/*_############################################################################
  _## 
  _##  snmp_proxy_mib.h  
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


#ifndef _snmp_proxy_mib_h
#define _snmp_proxy_mib_h


#include <agent_pp/mib.h>
#include <agent_pp/snmp_textual_conventions.h>


#define oidSnmpProxyTable                "1.3.6.1.6.3.14.1.2"
#define oidSnmpProxyEntry                "1.3.6.1.6.3.14.1.2.1"
#define oidSnmpProxyName                 "1.3.6.1.6.3.14.1.2.1.1"
#define oidSnmpProxyType                 "1.3.6.1.6.3.14.1.2.1.2"
#define oidSnmpProxyContextEngineID      "1.3.6.1.6.3.14.1.2.1.3"
#define oidSnmpProxyContextName          "1.3.6.1.6.3.14.1.2.1.4"
#define oidSnmpProxyTargetParamsIn       "1.3.6.1.6.3.14.1.2.1.5"
#define oidSnmpProxySingleTargetOut      "1.3.6.1.6.3.14.1.2.1.6"
#define oidSnmpProxyMultipleTargetOut    "1.3.6.1.6.3.14.1.2.1.7"
#define oidSnmpProxyStorageType          "1.3.6.1.6.3.14.1.2.1.8"
#define oidSnmpProxyRowStatus            "1.3.6.1.6.3.14.1.2.1.9"

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/**
 *  snmpProxyType
 *
"The type of message that may be forwarded using
 the translation parameters defined by this entry."
 */



/**
 *  snmpProxyContextEngineID
 *
"The contextEngineID contained in messages that
 may be forwarded using the translation parameters
 defined by this entry."
 */


/**
 *  snmpProxyContextName
 *
"The contextName contained in messages that may be
 forwarded using the translation parameters defined
 by this entry.

 This object is optional, and if not supported, the
 contextName contained in a message is ignored when
 selecting an entry in the snmpProxyTable."
 */


/**
 *  snmpProxyTargetParamsIn
 *
"This object selects an entry in the snmpTargetParamsTable.
 The selected entry is used to determine which row of the
 snmpProxyTable to use for forwarding received messages."
 */



/**
 *  snmpProxySingleTargetOut
 *
"This object selects a management target defined in the
 snmpTargetAddrTable (in the SNMP-TARGET-MIB). The
 selected target is defined by an entry in the
 snmpTargetAddrTable whose index value (snmpTargetAddrName)
 is equal to this object.

 This object is only used when selection of a single
 target is required (i.e. when forwarding an incoming
 read or write request)."
 */


/**
 *  snmpProxyMultipleTargetOut
 *
"This object selects a set of management targets defined
 in the snmpTargetAddrTable (in the SNMP-TARGET-MIB).

 This object is only used when selection of multiple
 targets is required (i.e. when forwarding an incoming
 notification)."
 */


/**
 *  snmpProxyRowStatus
 *
"The status of this conceptual row.

 To create a row in this table, a manager must
 set this object to either createAndGo(4) or
 createAndWait(5).

 The following objects may not be modified while the
 value of this object is active(1):
 - snmpProxyType
 - snmpProxyContextEngineID
 - snmpProxyContextName
 - snmpProxyTargetParamsIn
 - snmpProxySingleTargetOut
 - snmpProxyMultipleTargetOut"
 */



/**
 *  snmpProxyEntry
 *
"A set of translation parameters used by a proxy forwarder
 application for forwarding SNMP messages.

 Entries in the snmpProxyTable are created and deleted
 using the snmpProxyRowStatus object."
 */


class AGENTPP_DECL snmpProxyEntry: public StorageTable {

 public:
	snmpProxyEntry();
	virtual ~snmpProxyEntry();

	static snmpProxyEntry* instance;
	virtual void       	set_row(MibTableRow* r, int p0, char* p1, 
					char* p2, char* p3, char* p4, 
					char* p5, int p6, int p7);
 private:
	char* 	path;
};


class AGENTPP_DECL snmp_proxy_mib: public MibGroup
{
  public:
	snmp_proxy_mib();
	virtual ~snmp_proxy_mib() { }
};

#ifdef AGENTPP_NAMESPACE
}
#endif

/**
 * snmp_proxy_mib.h generated by AgentGen 1.1.3 for AGENT++v3 
 * Sun Nov 14 22:03:51 GMT+03:30 1999.
 */


#endif


