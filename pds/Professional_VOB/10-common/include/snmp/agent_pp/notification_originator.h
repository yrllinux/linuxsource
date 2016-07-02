/*_############################################################################
  _## 
  _##  notification_originator.h  
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

#ifndef _notification_originator_h_
#define _notification_originator_h_

#include <agent_pp/agent++.h>

#define mpV1  0
#define mpV2c 1
#define mpV3  3

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/*--------------------- class NotificationOriginator ---------------------*/

/**
 * The NotificationOriginator provides services to send traps and 
 * notifications by using the SNMP-TARGET-MIB and the SNMP-NOTIFICATION-MIB.
 * NotificationOriginator is typically used outside the main loop of an 
 * agent's request handling.
 * 
 * @author Frank Fock
 * @version 3.5.10
 */

class AGENTPP_DECL NotificationOriginator: public NotificationSender {

 public:
	/**
	 * Constructs a notification originator instance.
	 */
	NotificationOriginator();

	/**
	 * Destructor.
	 */
	virtual ~NotificationOriginator();

	/**
	 * Generate a notification message.
	 *
	 * @param vbs
	 *    an array of variable bindings - the payload of the notification.
	 * @param size
	 *    the size of the above array.
	 * @param id
	 *    the trap oid which identifies the notification.
	 * @param enterprise
	 *    the enterprise oid. For v2,v3 and enterprise specific v1 traps
	 *    this parameter should be "" (empty OID).
	 * @param contextName
	 *    the context in which the trap occured. 
	 */			       
	void    generate(Vbx*, int, const Oidx&, const Oidx&, const NS_SNMP OctetStr&);

	/**
	 * Generate a notification message.
	 *
	 * @param vbs
	 *    an array of variable bindings - the payload of the notification.
	 * @param size
	 *    the size of the above array.
	 * @param id
	 *    the trap oid which identifies the notification.
	 * @param sysUpTime
	 *    the timestamp to be used
	 * @param contextName
	 *    the context in which the trap occured. 
	 */			       
	void    generate(Vbx*, int, const Oidx&, 
			 unsigned int, const NS_SNMP OctetStr&);

	/**
	 * Send a notification. This implements the NotificationSender 
	 * interface.
	 *
	 * @param context
	 *    the context originating the notification ("" for the default 
	 *    context).
	 * @param trapoid
	 *    the oid of the notification.
	 * @param vbs
	 *    an array of variable bindings.
	 * @param size
	 *    the size of the above variable binding array.
	 * @param timestamp
	 *    an optional timestamp.
	 * @return 
	 *    SNMP_ERROR_SUCCESS if the notification could be sent
	 *    successfully, otherwise an appropriate SNMP error is
	 *    returned.
	 */
	virtual int		notify(const NS_SNMP OctetStr&, const Oidx&,
				       Vbx*, int, unsigned int=0); 
	
	/**
	 * Make all necessary entries in snmpTargetAddressTable,
	 * snmpTargetParamsTable, and snmpNotifyTable for the given
	 * v1 trap destination. All rows added will be named defaultV1Trap.
	 * The community used for generated traps will be public.
	 *
	 * @param targetAddress
	 *    an UDP address.
	 * @return
	 *    TRUE if the operation has been successful, FALSE otherwise.
	 */
	virtual boolean add_v1_trap_destination(const NS_SNMP UdpAddress& dest);
	virtual boolean add_v2_trap_destination(const NS_SNMP UdpAddress& addr);
	virtual boolean add_v3_trap_destination(const NS_SNMP UdpAddress& addr);

#ifdef _SNMPv3
	/**
	 * Sets the local engine ID to be used when sending notifications.
	 *
	 * @param engineID
	 *    an OctetStr instance.
	 */
	void		set_local_engine_id(const NS_SNMP OctetStr& id)
	    { if (localEngineID) { delete localEngineID; } 
	      localEngineID = new NS_SNMP OctetStr(id); }
#endif

 protected:
	
	class NotificationOriginatorParams {
	public:
		NotificationOriginatorParams(Vbx* _vbs, int _size, const Oidx& _id,
				unsigned int _timestamp,
				const Oidx& _enterprise, 
				const NS_SNMP OctetStr& _contextName) :
				vbs(_vbs), size(_size), id(_id), timestamp(_timestamp),
				enterprise(_enterprise), contextName(_contextName)
				{ target = 0; }

		Vbx* vbs;
		int size;
		Oidx id;
		unsigned int timestamp;
		Oidx enterprise;
		NS_SNMP OctetStr contextName;
		NS_SNMP OctetStr securityName;
		int securityModel;
		int securityLevel;
		int mpModel;
#ifdef _SNMPv3
		NS_SNMP UTarget* target;
#else
		NS_SNMP CTarget* target;
#endif

	private:
		NotificationOriginatorParams();
	};

	/**
	 * Generate a notification message.
	 *
	 * @param vbs
	 *    an array of variable bindings - the payload of the notification.
	 * @param size
	 *    the size of the above array.
	 * @param id
	 *    the trap oid which identifies the notification.
	 * @param sysUpTime
	 *    the timestamp to be used
	 * @param enterprise
	 *    the enterprise oid. For v2,v3 and enterprise specific v1 traps
	 *    this parameter should be "" (empty OID).
	 * @param contextName
	 *    the context in which the trap occured. 
	 */			       
	int    generate(Vbx*, int, const Oidx&, unsigned int, 
			const Oidx&, const NS_SNMP OctetStr&);

	/**
	 * Check notification access for a management target.
	 * Call this to validate access before sending the notificaiton.  The
	 * NotificationOriginatorParams parameter must have the vbs, size, id,
	 * and contextName parameters filled in.  Upon a TRUE return, the
	 * securityName, securityModel, securityLevel, mpModel, and target
	 * parameters will be filled in.  If TRUE is returned, the caller is
	 * responsible to delete the target object in the
	 * NotificationOriginatorParams object (after sending the notification).
	 *
	 * @param cur
	 *    the selected entry in the snmpTargetAddrTable
	 * @param nop
	 *    the notification originator parameters
	 * @return
	 *    TRUE if access is okay, FALSE otherwise
	 */			       
	boolean NotificationOriginator::check_access(ListCursor<MibTableRow>& cur, NotificationOriginatorParams& nop);

	/**
	 * Send a notification to a valid target.
	 * Call this only after validating access by calling the check_access
	 * method first.  The NotificationOriginatorParams parameter must have
	 * the vbs, size, id, timestamp, enterprise, contextName, securityName,
	 * securityLevel, mpModel, and target parameters filled in.
	 *
	 * @param cur
	 *    the selected entry in the snmpTargetAddrTable
	 * @param nop
	 *    the notification originator parameters
	 * @param notify
	 *    specifies the notification type (trap or inform)
	 * @return
	 *    The result from calling SnmpRequestV3::send or
	 *    SnmpRequest::process_trap
	 */			       
	int NotificationOriginator::send_notify(ListCursor<MibTableRow>& cur, NotificationOriginatorParams& nop, int notify);

#ifdef _SNMPv3
	NS_SNMP OctetStr*	localEngineID;
#endif
};
#ifdef AGENTPP_NAMESPACE
}
#endif
#endif
