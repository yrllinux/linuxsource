/*_############################################################################
  _## 
  _##  mib_proxy.h  
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

#ifndef mib_proxy_h_
#define mib_proxy_h_

#include <agent_pp/agent++.h>
#include <agent_pp/mib.h>
#ifdef _SNMPv3
#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/snmp_proxy_mib.h>
#endif

#ifndef _PROXY_FORWARDER

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/*--------------------------- class MibProxy -----------------------------*/

/**
 * The MibProxy class represents a sub-tree within an agents MIB whose
 * nodes and their values are proxied (forwarded) from another 
 * SNMPv1/v2c agent. The following figure displays a possible scenario:
 * <pre>
 *    +--------------+        +----------+       +-----------+
 *    |    Manager   |        | AGENT++  |       | Any v1 or |
 *    |              |--------| MibProxy |-------| v2c agent |
 *    | v1/v2c or v3 |        |          |       |           |
 *    +--------------+        +----------+       +-----------+
 * </pre>
 * SNMP v3 support for MibProxy can be easily added by sub-classing.
 *
 * @author Frank Fock
 * @version 3.02
 */

class AGENTPP_DECL MibProxy: public MibEntry {

public:
	/** 
	 * Default constructor
	 */
	MibProxy();

	/**
	 * Copy constructor
	 *
	 * @param other
	 *    another MibProxy instance
	 */
	MibProxy(const MibProxy& other);

	/**
	 * Simple proxy registration
	 *
	 * @param root
	 *    the oid of the root of the proxied MIB portion
	 * @param access
	 *    the max-access for the whole sub-tree
	 * @param source
	 *    the UDP address of the SNMP agent to whom requests for oids
	 *    within this MibProxy sub-tree should be redirected
	 */  
	MibProxy(const Oidx&, mib_access, const UdpAddress&);

	/**
	 * Proxy registration with oid translation
	 *
	 * @param root
	 *    the oid of the root of the proxied MIB portion
	 * @param access
	 *    the max-access for the whole sub-tree
	 * @param translated_root
	 *    root will be replaced by translated_root for requests
	 *    sent out to the remote agent (for example if root="1.3.6.1.3"
	 *    and translated_root="1.3.6.1.2" all oids of mgmt of the 
	 *    remote agent would become experimental objects of the proxy
	 *    agent
	 * @param source
	 *    the UDP address of the SNMP agent to whom requests for oids
	 *    within this MibProxy sub-tree should be redirected
	 */   
	MibProxy(const Oidx&, mib_access, const Oidx&, const UdpAddress&);

	/**
	 * Destructor
	 */
	virtual ~MibProxy() { }

	/**
	 * Return type of the MIB entry
	 *
	 * @return
	 *    AGENTPP_PROXY
	 */
	virtual mib_type	type() const  { return AGENTPP_PROXY; }

	/**
	 * Clone 
	 *
	 * @return
	 *    a pointer to a clone of the receiver
	 */
	virtual MibEntry*	clone() { return new MibProxy(*this); }

	/**
	 * Return the immediate successor of the greatest object identifier 
	 * within the receiver's scope
	 * 
	 * @return 
	 *    a pointer to the non including upper bound of the receiver's 
	 *    scope
	 */
	virtual OidxPtr		max_key();

	/**
	 * Find the successor of the given OID in the proxied MIB view
	 * and cache its value for a possibly following get_next_request 
	 * call 
	 * 
	 * @param id
	 *    an oid less or equal than max_key()
	 * @param request
	 *    a pointer to the Request instance for in whose context the
	 *    find_next is issued.
	 * @return
	 *    an oid greater than id or an empty oid (zero length) if 
	 *    an successor could not be determined or would be greater than
	 *    max_key()
	 */ 
	virtual Oidx		find_succ(const Oidx&, Request* r = 0);

	/**
	 * Check whether the reciever node contains any instance of a
	 * managed object.
	 *
	 * @return TRUE if the node currently manages an instance.
	 */  
	virtual boolean		is_empty() 
				{ return ((find_succ(*key()).len() == 0) ||
					  (lastNext.get_oid() >= *max_key()));}

	virtual void		get_request(Request*, int);
	virtual int		commit_set_request(Request*, int);
	virtual int		prepare_set_request(Request*, int&);
	virtual void		get_next_request(Request*, int);

	/**
	 * Set the READ and WRITE community for outgoing requests, 
	 * respectively
	 *
	 * @param access_type
	 *    the access type associated with the community - currently
	 *    only READING and WRITING are supported
	 * @param community
	 *    the community to be used for the specified access type
	 */ 
	void			set_community(access_types a, 
					      const OctetStr& c) 
					{ community[a] = c; }

protected:
	virtual Oidx		translate(const Oidx&);
	virtual Oidx		backward_translate(const Oidx&);

	void		determineDefaultRange(const Oidx&);

	UdpAddress	source;
	Oidx		translation;
	boolean		translating;

	OctetStr	community[WRITING+1];

	Oidx		range;

	Vbx		lastNext;
	int		lastNextStatus;
};

#ifdef _SNMPv3

/*--------------------------- class MibProxyV3 -----------------------------*/

/**
 * The MibProxyV3 class represents a sub-tree within an agents MIB whose
 * nodes and their values are proxied (forwarded) from another 
 * SNMPv1/v2c agent. The following figure displays a possible scenario:
 *
 *    ----------------        --------------       ---------------
 *    |    Manager   |        | AGENT++    |       | Any v1,v2c, |
 *    |              |--------| MibProxyV3 |-------| or v3 agent |
 *    | v1/v2c or v3 |        |            |       |             |
 *    ----------------        --------------       ---------------
 *
 * TODO: Notification support.
 *
 * @author Frank Fock
 * @version 3.4.1
 */

class AGENTPP_DECL MibProxyV3: public MibEntry {

public:
	/** 
	 * Default constructor
	 */
	MibProxyV3(const Oidx&, mib_access);

	/**
	 * Copy constructor
	 *
	 * @param other
	 *    another MibProxyV3 instance
	 */
	MibProxyV3(const MibProxyV3& other);

	/**
	 * Simple proxy registration
	 *
	 * @param proxyTable
	 *    a pointer to a snmpProxyEntry object representing the
	 *    proxy information described in the SNMP-PROXY-MIB
	 * @param root
	 *    the oid of the root of the proxied MIB portion
	 * @param access
	 *    the max-access for the whole sub-tree
	 */  
	MibProxyV3(snmpProxyEntry*, const Oidx&, mib_access);

	/**
	 * Creates a proxy forwared application for that will be able
	 * to forward messages with the given contextEngineID.
	 *
	 * @param contextEngineID
	 *    an OctetStr denoting a contextEngineID. 
	 */
	MibProxyV3(const OctetStr&); 

	/**
	 * Destructor
	 */
	virtual ~MibProxyV3() { }

	/**
	 * Return type of the MIB entry
	 *
	 * @return
	 *    AGENTPP_PROXY
	 */
	virtual mib_type	type() const  { return AGENTPP_PROXY; }

	/**
	 * Clone 
	 *
	 * @return
	 *    a pointer to a clone of the receiver
	 */
	virtual MibEntry*	clone() { return new MibProxyV3(*this); }

	/**
	 * Return the immediate successor of the greatest object identifier 
	 * within the receiver's scope
	 * 
	 * @return 
	 *    a pointer to the non including upper bound of the receiver's 
	 *    scope
	 */
	virtual OidxPtr		max_key();

	/**
	 * Find the successor of the given OID in the proxied MIB view
	 * and cache its value for a possibly following get_next_request 
	 * call 
	 * 
	 * @param id
	 *    an oid less or equal than max_key()
	 * @param request
	 *    a pointer to the Request instance for in whose context the
	 *    find_next is issued.
	 * @return
	 *    an oid greater than id or an empty oid (zero length) if 
	 *    an successor could not be determined or would be greater than
	 *    max_key()
	 */ 
	virtual Oidx		find_succ(const Oidx&, Request* r = 0);

	virtual void		get_request(Request*, int);
	virtual int		commit_set_request(Request*, int);
	virtual int		prepare_set_request(Request*, int&);
	virtual void		get_next_request(Request*, int);


protected:

	OidList<MibTableRow>*	get_matches(Request*);
	boolean			match_target_params(Request*, const OctetStr&);
	int			process_single(Pdux&, Request*);

	void			check_references();
	void			determineDefaultRange(const Oidx&);

	snmpProxyEntry*		myProxyInfo;
	
	Oidx		range;

	Vbx		lastNext;
	int		lastNextStatus;
};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif

#endif
#endif


