/*_############################################################################
  _## 
  _##  notification_log_mib.h  
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



//--AgentGen BEGIN=_BEGIN
#include <agent_pp/agent++.h>
#ifdef _SNMPv3
#include <agent_pp/mib_complex_entry.h>
//--AgentGen END


#ifndef _notification_log_mib_h
#define _notification_log_mib_h


#include <agent_pp/mib.h>

#include <agent_pp/snmp_textual_conventions.h>
#include <agent_pp/notification_originator.h>


#define oidNlmConfigGlobalEntryLimit     "1.3.6.1.2.1.92.1.1.1.0"
#define oidNlmConfigGlobalAgeOut         "1.3.6.1.2.1.92.1.1.2.0"
#define oidNlmConfigLogTable             "1.3.6.1.2.1.92.1.1.3"
#define oidNlmConfigLogEntry             "1.3.6.1.2.1.92.1.1.3.1"
#define oidNlmLogName                    "1.3.6.1.2.1.92.1.1.3.1.1"
#define colNlmLogName                    "1"
#define oidNlmConfigLogFilterName        "1.3.6.1.2.1.92.1.1.3.1.2"
#define colNlmConfigLogFilterName        "2"
#define oidNlmConfigLogEntryLimit        "1.3.6.1.2.1.92.1.1.3.1.3"
#define colNlmConfigLogEntryLimit        "3"
#define oidNlmConfigLogAdminStatus       "1.3.6.1.2.1.92.1.1.3.1.4"
#define colNlmConfigLogAdminStatus       "4"
#define oidNlmConfigLogOperStatus        "1.3.6.1.2.1.92.1.1.3.1.5"
#define colNlmConfigLogOperStatus        "5"
#define oidNlmConfigLogStorageType       "1.3.6.1.2.1.92.1.1.3.1.6"
#define colNlmConfigLogStorageType       "6"
#define oidNlmConfigLogEntryStatus       "1.3.6.1.2.1.92.1.1.3.1.7"
#define colNlmConfigLogEntryStatus       "7"
#define oidNlmStatsGlobalNotificationsLogged "1.3.6.1.2.1.92.1.2.1.0"
#define oidNlmStatsGlobalNotificationsBumped "1.3.6.1.2.1.92.1.2.2.0"
#define oidNlmStatsLogTable              "1.3.6.1.2.1.92.1.2.3"
#define oidNlmStatsLogEntry              "1.3.6.1.2.1.92.1.2.3.1"
#define oidNlmStatsLogNotificationsLogged "1.3.6.1.2.1.92.1.2.3.1.1"
#define colNlmStatsLogNotificationsLogged "1"
#define oidNlmStatsLogNotificationsBumped "1.3.6.1.2.1.92.1.2.3.1.2"
#define colNlmStatsLogNotificationsBumped "2"
#define oidNlmLogTable                   "1.3.6.1.2.1.92.1.3.1"
#define oidNlmLogEntry                   "1.3.6.1.2.1.92.1.3.1.1"
#define oidNlmLogIndex                   "1.3.6.1.2.1.92.1.3.1.1.1"
#define colNlmLogIndex                   "1"
#define oidNlmLogTime                    "1.3.6.1.2.1.92.1.3.1.1.2"
#define colNlmLogTime                    "2"
#define oidNlmLogDateAndTime             "1.3.6.1.2.1.92.1.3.1.1.3"
#define colNlmLogDateAndTime             "3"
#define oidNlmLogEngineID                "1.3.6.1.2.1.92.1.3.1.1.4"
#define colNlmLogEngineID                "4"
#define oidNlmLogEngineTAddress          "1.3.6.1.2.1.92.1.3.1.1.5"
#define colNlmLogEngineTAddress          "5"
#define oidNlmLogEngineTDomain           "1.3.6.1.2.1.92.1.3.1.1.6"
#define colNlmLogEngineTDomain           "6"
#define oidNlmLogContextEngineID         "1.3.6.1.2.1.92.1.3.1.1.7"
#define colNlmLogContextEngineID         "7"
#define oidNlmLogContextName             "1.3.6.1.2.1.92.1.3.1.1.8"
#define colNlmLogContextName             "8"
#define oidNlmLogNotificationID          "1.3.6.1.2.1.92.1.3.1.1.9"
#define colNlmLogNotificationID          "9"
#define oidNlmLogVariableTable           "1.3.6.1.2.1.92.1.3.2"
#define oidNlmLogVariableEntry           "1.3.6.1.2.1.92.1.3.2.1"
#define oidNlmLogVariableIndex           "1.3.6.1.2.1.92.1.3.2.1.1"
#define colNlmLogVariableIndex           "1"
#define oidNlmLogVariableID              "1.3.6.1.2.1.92.1.3.2.1.2"
#define colNlmLogVariableID              "2"
#define oidNlmLogVariableValueType       "1.3.6.1.2.1.92.1.3.2.1.3"
#define colNlmLogVariableValueType       "3"
#define oidNlmLogVariableCounter32Val    "1.3.6.1.2.1.92.1.3.2.1.4"
#define colNlmLogVariableCounter32Val    "4"
#define oidNlmLogVariableUnsigned32Val   "1.3.6.1.2.1.92.1.3.2.1.5"
#define colNlmLogVariableUnsigned32Val   "5"
#define oidNlmLogVariableTimeTicksVal    "1.3.6.1.2.1.92.1.3.2.1.6"
#define colNlmLogVariableTimeTicksVal    "6"
#define oidNlmLogVariableInteger32Val    "1.3.6.1.2.1.92.1.3.2.1.7"
#define colNlmLogVariableInteger32Val    "7"
#define oidNlmLogVariableOctetStringVal  "1.3.6.1.2.1.92.1.3.2.1.8"
#define colNlmLogVariableOctetStringVal  "8"
#define oidNlmLogVariableIpAddressVal    "1.3.6.1.2.1.92.1.3.2.1.9"
#define colNlmLogVariableIpAddressVal    "9"
#define oidNlmLogVariableOidVal          "1.3.6.1.2.1.92.1.3.2.1.10"
#define colNlmLogVariableOidVal          "10"
#define oidNlmLogVariableCounter64Val    "1.3.6.1.2.1.92.1.3.2.1.11"
#define colNlmLogVariableCounter64Val    "11"
#define oidNlmLogVariableOpaqueVal       "1.3.6.1.2.1.92.1.3.2.1.12"
#define colNlmLogVariableOpaqueVal       "12"

#define nNlmConfigLogFilterName          0
#define cNlmConfigLogFilterName          2
#define nNlmConfigLogEntryLimit          1
#define cNlmConfigLogEntryLimit          3
#define nNlmConfigLogAdminStatus         2
#define cNlmConfigLogAdminStatus         4
#define nNlmConfigLogOperStatus          3
#define cNlmConfigLogOperStatus          5
#define nNlmConfigLogStorageType         4
#define cNlmConfigLogStorageType         6
#define nNlmConfigLogEntryStatus         5
#define cNlmConfigLogEntryStatus         7
#define nNlmStatsLogNotificationsLogged  0
#define cNlmStatsLogNotificationsLogged  1
#define nNlmStatsLogNotificationsBumped  1
#define cNlmStatsLogNotificationsBumped  2
#define nNlmLogTime                      0
#define cNlmLogTime                      2
#define nNlmLogDateAndTime               1
#define cNlmLogDateAndTime               3
#define nNlmLogEngineID                  2
#define cNlmLogEngineID                  4
#define nNlmLogEngineTAddress            3
#define cNlmLogEngineTAddress            5
#define nNlmLogEngineTDomain             4
#define cNlmLogEngineTDomain             6
#define nNlmLogContextEngineID           5
#define cNlmLogContextEngineID           7
#define nNlmLogContextName               6
#define cNlmLogContextName               8
#define nNlmLogNotificationID            7
#define cNlmLogNotificationID            9
#define nNlmLogVariableID                0
#define cNlmLogVariableID                2
#define nNlmLogVariableValueType         1
#define cNlmLogVariableValueType         3
#define nNlmLogVariableCounter32Val      2
#define cNlmLogVariableCounter32Val      4
#define nNlmLogVariableUnsigned32Val     3
#define cNlmLogVariableUnsigned32Val     5
#define nNlmLogVariableTimeTicksVal      4
#define cNlmLogVariableTimeTicksVal      6
#define nNlmLogVariableInteger32Val      5
#define cNlmLogVariableInteger32Val      7
#define nNlmLogVariableOctetStringVal    6
#define cNlmLogVariableOctetStringVal    8
#define nNlmLogVariableIpAddressVal      7
#define cNlmLogVariableIpAddressVal      9
#define nNlmLogVariableOidVal            8
#define cNlmLogVariableOidVal            10
#define nNlmLogVariableCounter64Val      9
#define cNlmLogVariableCounter64Val      11
#define nNlmLogVariableOpaqueVal         10
#define cNlmLogVariableOpaqueVal         12


//--AgentGen BEGIN=_INCLUDE
#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

class AGENTPP_DECL nlmConfigLogOperStatus: public MibLeaf {

public:
	nlmConfigLogOperStatus(const Oidx&);
	virtual ~nlmConfigLogOperStatus();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
};

//--AgentGen END


/**
 *  nlmConfigGlobalEntryLimit
 *
 * "The maximum number of notification entries that may be held
 * in nlmLogTable for all nlmLogNames added together.  A particular
 * setting does not guarantee that much data can be held.
 * 
 * If an application changes the limit while there are
 * Notifications in the log, the oldest Notifications MUST be
 * discarded to bring the log down to the new limit - thus the
 * value of nlmConfigGlobalEntryLimit MUST take precedence over
 * the values of nlmConfigGlobalAgeOut and nlmConfigLogEntryLimit,
 * even if the Notification being discarded has been present for
 * fewer minutes than the value of nlmConfigGlobalAgeOut, or if
 * the named log has fewer entries than that specified in
 * nlmConfigLogEntryLimit.
 * 
 * A value of 0 means no limit.
 * 
 * Please be aware that contention between multiple managers
 * trying to set this object to different values MAY affect the
 * reliability and completeness of data seen by each manager."
 */


class AGENTPP_DECL nlmConfigGlobalEntryLimit: public MibLeaf {

public:
	nlmConfigGlobalEntryLimit();
	virtual ~nlmConfigGlobalEntryLimit();

	static nlmConfigGlobalEntryLimit* instance;

	virtual void       	get_request(Request*, int);
	virtual unsigned long       	get_state();
	virtual void       	set_state(unsigned long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual boolean    	value_ok(const Vbx&);

//--AgentGen BEGIN=nlmConfigGlobalEntryLimit
//--AgentGen END
};


/**
 *  nlmConfigGlobalAgeOut
 *
 * "The number of minutes a Notification SHOULD be kept in a log
 * before it is automatically removed.
 * 
 * If an application changes the value of nlmConfigGlobalAgeOut,
 * Notifications older than the new time MAY be discarded to meet the
 * new time.
 * 
 * A value of 0 means no age out.
 * 
 * Please be aware that contention between multiple managers
 * trying to set this object to different values MAY affect the
 * reliability and completeness of data seen by each manager."
 */


class AGENTPP_DECL nlmConfigGlobalAgeOut: public MibLeaf {

public:
	nlmConfigGlobalAgeOut();
	virtual ~nlmConfigGlobalAgeOut();

	static nlmConfigGlobalAgeOut* instance;

	virtual void       	get_request(Request*, int);
	virtual unsigned long       	get_state();
	virtual void       	set_state(unsigned long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual boolean    	value_ok(const Vbx&);

//--AgentGen BEGIN=nlmConfigGlobalAgeOut
//--AgentGen END
};


/**
 *  nlmConfigLogFilterName
 *
 * "A value of snmpNotifyFilterProfileName as used as an index
 * into the snmpNotifyFilterTable in the SNMP Notification MIB,
 * specifying the locally or remotely originated Notifications
 * to be filtered out and not logged in this log.
 * 
 * A zero-length value or a name that does not identify an
 * existing entry in snmpNotifyFilterTable indicate no
 * Notifications are to be logged in this log."
 */


class AGENTPP_DECL nlmConfigLogFilterName: public MibLeaf {

public:
	nlmConfigLogFilterName(const Oidx&);
	virtual ~nlmConfigLogFilterName();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual NS_SNMP OctetStr       	get_state();
	virtual void       	set_state(const NS_SNMP OctetStr&);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual boolean    	value_ok(const Vbx&);

//--AgentGen BEGIN=nlmConfigLogFilterName
//--AgentGen END
};


/**
 *  nlmConfigLogEntryLimit
 *
 * "The maximum number of notification entries that can be held in
 * nlmLogTable for this named log.  A particular setting does not
 * guarantee that that much data can be held.
 * 
 * If an application changes the limit while there are
 * Notifications in the log, the oldest Notifications are discarded
 * to bring the log down to the new limit.
 * A value of 0 indicates no limit.
 * 
 * Please be aware that contention between multiple managers
 * trying to set this object to different values MAY affect the
 * reliability and completeness of data seen by each manager."
 */


class AGENTPP_DECL nlmConfigLogEntryLimit: public MibLeaf {

public:
	nlmConfigLogEntryLimit(const Oidx&);
	virtual ~nlmConfigLogEntryLimit();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual unsigned long       	get_state();
	virtual void       	set_state(unsigned long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual boolean    	value_ok(const Vbx&);

//--AgentGen BEGIN=nlmConfigLogEntryLimit
//--AgentGen END
};


/**
 *  nlmConfigLogAdminStatus
 *
 * "Control to enable or disable the log without otherwise
 * disturbing the log's entry.
 * 
 * Please be aware that contention between multiple managers
 * trying to set this object to different values MAY affect the
 * reliability and completeness of data seen by each manager."
 */


class AGENTPP_DECL nlmConfigLogAdminStatus: public MibLeaf {

public:
	nlmConfigLogAdminStatus(const Oidx&);
	virtual ~nlmConfigLogAdminStatus();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual boolean    	value_ok(const Vbx&);
	enum labels {
		e_enabled = 1,
		e_disabled = 2 };

//--AgentGen BEGIN=nlmConfigLogAdminStatus
//--AgentGen END
};


/**
 *  nlmConfigLogStorageType
 *
 * "The storage type of this conceptual row."
 */


class AGENTPP_DECL nlmConfigLogStorageType: public StorageType {

public:
	nlmConfigLogStorageType(const Oidx&);
	virtual ~nlmConfigLogStorageType();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual boolean    	value_ok(const Vbx&);
	enum labels {
		e_other = 1,
		e_volatile = 2,
		e_nonVolatile = 3,
		e_permanent = 4,
		e_readOnly = 5 };

//--AgentGen BEGIN=nlmConfigLogStorageType
//--AgentGen END
};


/**
 *  nlmConfigLogEntryStatus
 *
 * "Control for creating and deleting entries.  Entries may be
 * modified while active.
 * 
 * For non-null-named logs, the managed system records the security
 * credentials from the request that sets nlmConfigLogStatus
 * to 'active' and uses that identity to apply access control to
 * the objects in the Notification to decide if that Notification
 * may be logged."
 */


class AGENTPP_DECL nlmConfigLogEntryStatus: public snmpRowStatus {

public:
	nlmConfigLogEntryStatus(const Oidx&);
	virtual ~nlmConfigLogEntryStatus();

	virtual MibEntryPtr	clone();
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	enum labels {
		e_active = 1,
		e_notInService = 2,
		e_notReady = 3,
		e_createAndGo = 4,
		e_createAndWait = 5,
		e_destroy = 6 };

//--AgentGen BEGIN=nlmConfigLogEntryStatus
	virtual int		commit_set_request(Request*, int);
//--AgentGen END
};


/**
 *  nlmStatsGlobalNotificationsLogged
 *
 * "The number of Notifications put into the nlmLogTable.  This
 * counts a Notification once for each log entry, so a Notification
 *  put into multiple logs is counted multiple times."
 */


class AGENTPP_DECL nlmStatsGlobalNotificationsLogged: public MibLeaf {

public:
	nlmStatsGlobalNotificationsLogged();
	virtual ~nlmStatsGlobalNotificationsLogged();

	static nlmStatsGlobalNotificationsLogged* instance;

	virtual void       	get_request(Request*, int);
	virtual unsigned long       	get_state();
	virtual void       	set_state(unsigned long);
	virtual unsigned long 	inc();

//--AgentGen BEGIN=nlmStatsGlobalNotificationsLogged
	virtual boolean		is_volatile() { return TRUE; }
//--AgentGen END
};


/**
 *  nlmStatsGlobalNotificationsBumped
 *
 * "The number of log entries discarded to make room for a new entry
 * due to lack of resources or the value of nlmConfigGlobalEntryLimit
 * or nlmConfigLogEntryLimit.  This does not include entries discarded
 * due to the value of nlmConfigGlobalAgeOut."
 */


class AGENTPP_DECL nlmStatsGlobalNotificationsBumped: public MibLeaf {

public:
	nlmStatsGlobalNotificationsBumped();
	virtual ~nlmStatsGlobalNotificationsBumped();

	static nlmStatsGlobalNotificationsBumped* instance;

	virtual void       	get_request(Request*, int);
	virtual unsigned long       	get_state();
	virtual void       	set_state(unsigned long);
	virtual unsigned long 	inc();

//--AgentGen BEGIN=nlmStatsGlobalNotificationsBumped
	virtual boolean		is_volatile() { return TRUE; }
//--AgentGen END
};


/**
 *  nlmConfigLogEntry
 *
 * "A logging control entry.  Depending on the entry's storage type
 * entries may be supplied by the system or created and deleted by
 * applications using nlmConfigLogEntryStatus."
 */


class AGENTPP_DECL nlmConfigLogEntry: public StorageTable {

public:
	nlmConfigLogEntry();
	virtual ~nlmConfigLogEntry();

	static nlmConfigLogEntry* instance;

	virtual void        	row_added(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_delete(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_init(MibTableRow*, const Oidx&, MibTable*);
	virtual void       	set_row(MibTableRow* r, const NS_SNMP OctetStr& p0, unsigned long p1, long p2, long p3, long p4, long p5);

//--AgentGen BEGIN=nlmConfigLogEntry
//--AgentGen END
};


/**
 *  nlmStatsLogEntry
 *
 * "A Notification log statistics entry."
 */


class AGENTPP_DECL nlmStatsLogEntry: public MibTable {

public:
	nlmStatsLogEntry();
	virtual ~nlmStatsLogEntry();

	static nlmStatsLogEntry* instance;

	virtual void        	row_added(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_delete(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_init(MibTableRow*, const Oidx&, MibTable*);
	virtual void       	set_row(MibTableRow* r, unsigned long p0, unsigned long p1);

//--AgentGen BEGIN=nlmStatsLogEntry
	virtual boolean		is_volatile() { return TRUE; }
 protected:
	
//--AgentGen END
};


/**
 *  nlmLogEntry
 *
 * "A Notification log entry.
 * 
 * Entries appear in this table when Notifications occur and pass
 * filtering by nlmConfigLogFilterName and access control.  They are
 * removed to make way for new entries due to lack of resources or
 * the values of nlmConfigGlobalEntryLimit, nlmConfigGlobalAgeOut, or
 * nlmConfigLogEntryLimit.
 * 
 * If adding an entry would exceed nlmConfigGlobalEntryLimit or system
 * resources in general, the oldest entry in any log SHOULD be removed
 * to make room for the new one.
 * 
 * If adding an entry would exceed nlmConfigLogEntryLimit the oldest
 * entry in that log SHOULD be removed to make room for the new one.
 * 
 * Before the managed system puts a locally-generated Notification
 * into a non-null-named log it assures that the creator of the log
 * has access to the information in the Notification.  If not it
 * does not log that Notification in that log."
 */


class AGENTPP_DECL nlmLogEntry: public MibTable {

public:
	nlmLogEntry();
	virtual ~nlmLogEntry();

	static nlmLogEntry* instance;

	virtual void        	row_added(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_delete(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_init(MibTableRow*, const Oidx&, MibTable*);
	virtual void       	set_row(MibTableRow* r, unsigned long p0, const NS_SNMP OctetStr& p1, const NS_SNMP OctetStr& p2, const NS_SNMP OctetStr& p3, const char* p4, const NS_SNMP OctetStr& p5, const NS_SNMP OctetStr& p6, const char* p7);

//--AgentGen BEGIN=nlmLogEntry
	virtual boolean		is_volatile() { return TRUE; }
	
	boolean			check_access(const Pdux&, MibTableRow*);
	void			check_limits(List<MibTableRow>*);
	void			add_notification(const NS_SNMP SnmpTarget&, 
						 const Pdux&,
						 const NS_SNMP OctetStr&);

 protected:
	OidList<MibStaticEntry> logIndexes;
	List<MibTableRow>	entries;
	MibTable*		secInfo;
//--AgentGen END
};


/**
 *  nlmLogVariableEntry
 *
 * "A Notification log entry variable.
 * 
 * Entries appear in this table when there are variables in
 * the varbind list of a Notification in nlmLogTable."
 */


class AGENTPP_DECL nlmLogVariableEntry: public MibTable {

public:
	nlmLogVariableEntry();
	virtual ~nlmLogVariableEntry();

	static nlmLogVariableEntry* instance;

	virtual void        	row_added(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_delete(MibTableRow*, const Oidx&, MibTable*);
	virtual void        	row_init(MibTableRow*, const Oidx&, MibTable*);
	virtual void       	set_row(MibTableRow* r, const char* p0, long p1, unsigned long p2, unsigned long p3, unsigned long p4, long p5, const NS_SNMP OctetStr& p6, const char* p7, const char* p8, unsigned long p9hi, unsigned long p9lo
		, const NS_SNMP OctetStr& p10);

//--AgentGen BEGIN=nlmLogVariableEntry
	virtual boolean		is_volatile() { return TRUE; }
	void			add_variable(const Oidx&, unsigned int, 
					     const Vbx&);
//--AgentGen END
};


class AGENTPP_DECL notification_log_mib: public MibGroup
{
  public:
	notification_log_mib();
	virtual ~notification_log_mib() { }

//--AgentGen BEGIN=notification_log_mib
//--AgentGen END

};

//--AgentGen BEGIN=_END
#ifdef AGENTPP_NAMESPACE
}
#endif
#endif
//--AgentGen END


/**
 * notification_log_mib.h generated by AgentGen 1.6.2 for AGENT++v3.4 
 * Sun Jun 03 23:58:42 GMT+02:00 2001.
 */


#endif


