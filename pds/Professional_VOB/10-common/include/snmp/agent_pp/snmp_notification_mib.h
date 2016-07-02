/*_############################################################################
  _## 
  _##  snmp_notification_mib.h  
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

#ifndef _snmp_notification_mib_h_
#define _snmp_notification_mib_h_

#include <agent_pp/mib.h>
#include <agent_pp/snmp_textual_conventions.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/**
 *  snmpNotifyType
 *
"This object determines the type of notification to
 be generated for entries in the snmpTargetAddrTable
 selected by the corresponding instance of
 snmpNotifyTag. This value is only used when
 generating notifications, and is ignored when
 using the snmpTargetAddrTable for other purposes.

 If the value of this object is trap(1), then any
 messages generated for selected rows will contain
 Unconfirmed-Class PDUs.

 If the value of this object is inform(2), then any
 messages generated for selected rows will contain
 Confirmed-Class PDUs.

 Note that if an SNMP entity only supports
 generation of Unconfirmed-Class PDUs (and not
 Confirmed-Class PDUs), then this object may be
 read-only."
 */


/**
 *  snmpNotifyRowStatus
 *
"The status of this conceptual row.

 To create a row in this table, a manager must
 set this object to either createAndGo(4) or
 createAndWait(5)."
 */


/**
 *  snmpNotifyFilterProfileName
 *
"The name of the filter profile to be used when generating
 notifications using the corresponding entry in the
 snmpTargetAddrTable."
 */



/**
 *  snmpNotifyFilterProfileRowStatus
 *
"The status of this conceptual row.

 To create a row in this table, a manager must
 set this object to either createAndGo(4) or
 createAndWait(5).

 Until instances of all corresponding columns are
 appropriately configured, the value of the
 corresponding instance of the
 snmpNotifyFilterProfileRowStatus column is 'notReady'.

 In particular, a newly created row cannot be made
 active until the corresponding instance of
 snmpNotifyFilterProfileName has been set."
 */


/**
 *  snmpNotifyFilterMask
 *
"The bit mask which, in combination with the corresponding
 instance of snmpNotifyFilterSubtree, defines a family of
 subtrees which are included in or excluded from the
 filter profile.

 Each bit of this bit mask corresponds to a
 sub-identifier of snmpNotifyFilterSubtree, with the
 most significant bit of the i-th octet of this octet
 string value (extended if necessary, see below)
 corresponding to the (8*i - 7)-th sub-identifier, and
 the least significant bit of the i-th octet of this
 octet string corresponding to the (8*i)-th
 sub-identifier, where i is in the range 1 through 16.

 Each bit of this bit mask specifies whether or not
 the corresponding sub-identifiers must match when
 determining if an OBJECT IDENTIFIER matches this
 family of filter subtrees; a '1' indicates that an
 exact match must occur; a '0' indicates 'wild card',
 i.e., any sub-identifier value matches.

 Thus, the OBJECT IDENTIFIER X of an object instance
 is contained in a family of filter subtrees if, for
 each sub-identifier of the value of
 snmpNotifyFilterSubtree, either:

 the i-th bit of snmpNotifyFilterMask is 0, or

 the i-th sub-identifier of X is equal to the i-th
 sub-identifier of the value of
 snmpNotifyFilterSubtree.

 If the value of this bit mask is M bits long and
 there are more than M sub-identifiers in the
 corresponding instance of snmpNotifyFilterSubtree,
 then the bit mask is extended with 1's to be the
 required length.

 Note that when the value of this object is the
 zero-length string, this extension rule results in
 a mask of all-1's being used (i.e., no 'wild card'),
 and the family of filter subtrees is the one
 subtree uniquely identified by the corresponding
 instance of snmpNotifyFilterSubtree."
 */


/**
 *  snmpNotifyFilterType
 *
"This object indicates whether the family of filter subtrees
 defined by this entry are included in or excluded from a
 filter. A more detailed discussion of the use of this
 object can be found in section 6. of [SNMP-APPL]."
 */


/**
 *  snmpNotifyFilterRowStatus
 *
"The status of this conceptual row.

 To create a row in this table, a manager must
 set this object to either createAndGo(4) or
 createAndWait(5)."
 */


/**
 * snmpNotifyEntry
 *
 * An entry in this table selects a set of management targets
 * which should receive notifications, as well as the type of
 * notification which should be sent to each selected
 * management target.
 *
 * Entries in the snmpNotifyTable are created and
 * deleted using the snmpNotifyRowStatus object.
 *
 * @version 3.4
 */


class AGENTPP_DECL snmpNotifyEntry: public StorageTable {

 public:
	snmpNotifyEntry();
	virtual ~snmpNotifyEntry();

	/**
	 * Add a new entry to the reciever. The new entry is
	 * used to specify target addresses to send notifications
	 * to.
	 *
	 * @param name
	 *    a unique identifier used to index this table.
	 * @param tag
	 *    a tag value used to select entries in 
	 *    snmpTargetAddrTable.
	 * @param type
	 *    specifies whether to generate a Trap PDU (1)
	 *    or an Inform-PDU (2).
	 * @return
	 *    a pointer to the newly created entry or 0 if
	 *    a row with index name already exists.
	 */
	MibTableRow* add_entry(const NS_SNMP OctetStr&,
			       const NS_SNMP OctetStr&,
			       const int);

	static snmpNotifyEntry* instance;
};


/**
 *  snmpNotifyFilterProfileEntry
 *
"An entry in this table indicates the name of the filter
 profile to be used when generating notifications using
 the corresponding entry in the snmpTargetParamsTable.

 Entries in the snmpNotifyFilterProfileTable are created
 and deleted using the snmpNotifyFilterProfileRowStatus
 object."
 */


class AGENTPP_DECL snmpNotifyFilterProfileEntry: public StorageTable {

 public:
	snmpNotifyFilterProfileEntry();
	virtual ~snmpNotifyFilterProfileEntry();

	static snmpNotifyFilterProfileEntry* instance;
};


/**
 *  snmpNotifyFilterEntry
 *
"An element of a filter profile.

 Entries in the snmpNotifyFilterTable are created and
 deleted using the snmpNotifyFilterRowStatus object."
 */


class AGENTPP_DECL snmpNotifyFilterEntry: public StorageTable {

 public:
	snmpNotifyFilterEntry();
	virtual ~snmpNotifyFilterEntry();

	static snmpNotifyFilterEntry* instance;

	/**
	 * Checks whether an object identifier passes the notification
	 * filter specified by the given profile name (a pointer into
	 * the snmpNotifyFilterTable).
	 *
	 * @param target
	 *    an object identifier representing an index value of the
	 *    snmpTargetParamsTable. 
	 * @param oid
	 *    an object identifier to check. 
	 * @param vbs
	 *    an array of Vbx instances whose OIDs should be checked.
	 * @param vb_count
	 *    the number of the above variable bindings.
	 * @return 
	 *    TRUE if the object id passes the filter or if no appropiate
	 *    filter exists, in any other case return FALSE.
	 */
	static boolean passes_filter(const Oidx&, 
				     const Oidx&,
				     const Vbx*,
				     unsigned int);
};


class AGENTPP_DECL snmp_notification_mib: public MibGroup
{
  public:
	snmp_notification_mib();
	virtual ~snmp_notification_mib() { }
};

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif


