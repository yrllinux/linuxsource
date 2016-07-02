/*_############################################################################
  _## 
  _##  sim_mib.h  
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


#ifndef sim_mib_h_
#define sim_mib_h_

#include <agent_pp/agent++.h>
#include <agent_pp/mib.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/**
 * The class SimMib can be used to create simulation agents from
 * real agents. Simply use SimMib instead of Mib and add MIB objects
 * like MibLeaf and MibTable to it. Then the content of the MIB can be 
 * cloned from a SNMPv1/v2c agent.
 *
 * @author Frank Fock
 * @version 3.1
 */

class AGENTPP_DECL SimMib:public Mib 
{
 public:
	/**
	 * Default Constructor
	 */
	SimMib();

	/**
	 * Desctructor
	 */
	virtual ~SimMib();
};


/**
 * The class SimMibLeaf can be used to create simulation agents.
 *
 * @author Frank Fock
 * @version 3.1c
 */

class AGENTPP_DECL SimMibLeaf: public MibLeaf 
{
 public:
  /**
   * Default constructor.
   */
  SimMibLeaf();

  /**
   * Construct a MibLeaf object.
   *
   * @param o - The object identifier of the managed object. If it is part
   *            of a table, only the column subidentifier is given. If it
   *            is a scalar object the object identifier given includes the
   *            full registration oid including the suffix ".0".
   * @param a - The maximum access rights for the managed object.
   * @param s - The syntax of the managed object.
   * @see Oidx, mib_access
   */
  SimMibLeaf(const Oidx&, mib_access, NS_SNMP SmiUINT32);

  /**
   * Construct a MibLeaf object.
   *
   * @param o - The object identifier of the managed object. If it is part
   *            of a table, only the column subidentifier is given. If it
   *            is a scalar object the object identifier given includes the
   *            full registration oid including the suffix ".0".
   * @param a - The maximum access rights for the managed object.
   * @param s - A pointer to the SnmpSyntax object to be used for storing
   *            the value of the managed object. NOTE: Do not delete this
   *            pointer from outside MibLeaf. It will be deleted by ~MibLeaf
   * @see Oidx, mib_access
   */
  SimMibLeaf(const Oidx&, mib_access, NS_SNMP SnmpSyntax*);

  /**
   * Construct a MibLeaf object.
   *
   * @param o - The object identifier of the managed object. If it is part
   *            of a table, only the column subidentifier is given. If it
   *            is a scalar object the object identifier given includes the
   *            full registration oid including the suffix ".0".
   * @param a - The maximum access rights for the managed object.
   * @param s - A pointer to the SnmpSyntax object to be used for storing
   *            the value of the managed object. NOTE: Do not delete this
   *            pointer from outside MibLeaf. It will be deleted by ~MibLeaf
   * @param d - If TRUE the value of the object pointed by s will be used
   *            as default value, otherwise the managed object has no default
   *            value.
   * @see Oidx, mib_access
   */
  SimMibLeaf(const Oidx&, mib_access, NS_SNMP SnmpSyntax*, boolean);

  /**
   * Copy Constructor.
   */
  SimMibLeaf(const SimMibLeaf&);

  /**
   * Destructor.
   */
  virtual ~SimMibLeaf();

  /**
   * Clone the receiver object.
   * 
   * @return A pointer to the clone.
   */
  virtual MibEntryPtr	clone();

  /**
   * Return the maximum access rights for the managed object
   * represented by the receiver node.
   * 
   * @return The maximum access (one of the following values:
   *         NOACCESS, READONLY, READWRITE, or READCREATE)
   */
  virtual mib_access    get_access();

  /**
   * Set config mode. All SimMibLeaf objects acting as a leaf object
   * get access rights of read-write and all SimMibLeaf objects acting
   * as a columnar object get read-create access rights.
   */ 
  static void		set_config_mode();
  /**
   * Unset config mode. All SimMibLeaf objects get their original
   * access rights (i.e., those rights they got when they have been
   * constructed). 
   */ 
  static void		unset_config_mode();

  /**
   * Get the current config mode.
   *
   * @return 
   *    true if the config mode is set, false otherwise.
   */
  static boolean	get_config_mode();

 protected:

  static boolean	configMode;

};

class AGENTPP_DECL SimMibTable: public MibTable {
 public:
	SimMibTable(const Oidx& o, const index_info* i, 
		    unsigned int n): MibTable(o, i, n) { }
	virtual ~SimMibTable() { }
	
	virtual int check_creation(Request*, int&); 
};

class AGENTPP_DECL SimRowStatus: public snmpRowStatus {
 public:
	SimRowStatus(const Oidx& o, mib_access a): snmpRowStatus(o, a) { }
	virtual ~SimRowStatus() { }

	virtual boolean	       	transition_ok(const Vbx&);

	/**
	  * Return the maximum access rights for the managed object
	  * represented by the receiver node.
	  * 
	  * @return The maximum access (one of the following values:
	  *         NOACCESS, READONLY, READWRITE, or READCREATE)
	  */
	virtual mib_access    get_access();

	/**
	  * Clone the receiver object.
	  * 
	  * @return A pointer to the clone.
	  */
	virtual MibEntryPtr	clone();

};

/**********************************************************************
 *  
 *  class simSysUpTime
 * 
 **********************************************************************/

class AGENTPP_DECL simSysUpTime: public SimMibLeaf {

public:
	simSysUpTime();

	void		get_request(Request*, int);

	static time_t	get();
	static time_t	get_currentTime();	

private:
	static time_t	start;
};

/**********************************************************************
 *  
 *  class simSysGroup
 * 
 **********************************************************************/

class AGENTPP_DECL simSysGroup: public MibGroup {

public:

	simSysGroup();
};


#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
