/*_############################################################################
  _## 
  _##  mib_complex_entry.h  
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


#ifndef _mib_complex_entry_h_
#define _mib_complex_entry_h_

#include <agent_pp/mib_entry.h>
#include <agent_pp/mib.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/*------------------------ class MibComplexEntry ------------------------*/

/**
 * The MibComplexEntry class is an abstract class that represents complex
 * MIB nodes. Complex nodes access their managed objects (e.g., MibLeafs) 
 * via an implementation specific way. This interface can be used to 
 * implement light weight tables, i.e. tables that access their management
 * information through a proprietary protocol (other than SNMP or AgentX).
 *
 * @author Frank Fock
 * @version 3.4
 */
class AGENTPP_DECL MibComplexEntry: public MibEntry {
friend class Mib;
public:
	/**
	 * Construct a MibComplexEntry instance with a given 
	 * object identifier and maximum access rights.
	 * 
	 * @param oid
	 *    an object identifier.
	 * @param access
	 *    the maximum access rights for the receiver.
	 */   
	MibComplexEntry(const Oidx&, mib_access);

	/**
	 * Copy constructor.
	 * 
	 * @param other
	 *    another MibComplexEntry instance.
	 */
	MibComplexEntry(const MibComplexEntry&);

	/**
	 * Destructor
	 */
	virtual	~MibComplexEntry();


	/**
	 * Return the type of the receiver MIB node.
	 *
	 * @return 
	 *    One of the following types: AGENTPP_NONE, 
	 *    AGENTPP_PROXY, AGENTPP_LEAF, 
	 *    AGENTPP_TABLE, AGENTPP_GROUP, or AGENTPP_COMPLEX.
	 */
	virtual mib_type	type() const { return AGENTPP_COMPLEX; }

	/**
	 * Return a clone of the receiver.
	 *
	 * @return
	 *    a pointer to a clone of the MibComplexEntry object.  
	 */
	virtual MibEntry*	clone() = 0;

	/**
	 * Return the successor of a given object identifier within the 
	 * receiver's scope and the context of a given Request.
	 *
	 * @param oid
	 *    an object identifier
	 * @param request
	 *    a pointer to a Request instance.
	 * @return 
	 *    an object identifier if a successor could be found,
	 *    otherwise (if no successor exists or is out of scope) 
	 *    a zero length oid is returned
	 */
	virtual Oidx		find_succ(const Oidx&, Request* req = 0) = 0;

 	// interfaces dispatch table <-> management instrumentation
	
	/**
	 * Let the receiver process a SNMP GET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GET request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void		get_request(Request*, int) = 0;

	/**
	 * Let the receiver process a SNMP GETNEXT subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GETNEXT request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void		get_next_request(Request*, int) = 0;
	
	/**
	 * Let the receiver commit a SNMP SET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @return SNMP_ERROR_SUCCESS on success and 
	 *         SNMP_ERROR_COMITFAIL on failure.
	 */
	virtual int		commit_set_request(Request*, int) 
				  { return SNMP_ERROR_COMITFAIL; }

	/**
	 * Let the receiver prepare a SNMP SET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @return SNMP_ERROR_SUCCESS on success and SNMP_ERROR_WRONG_VALUE,
	 *         SNMP_ERROR_WRONG_TYPE, or 
	 *         SNMP_ERROR_NOT_WRITEABLE on failure.
	 */
	virtual int		prepare_set_request(Request*, int&) 
				  { return SNMP_ERROR_NOT_WRITEABLE; }

	/**
	 * Let the receiver undo a SNMP SET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 * @return SNMP_ERROR_SUCCESS on success and 
	 *         SNMP_ERROR_UNDO_FAIL on failure.
	 */
	virtual int	        undo_set_request(Request*, int&) 
				  { return SNMP_ERROR_SUCCESS; }

	/**
	 * Clean up resources used for performing (or undoing) set requests. 
	 *
	 * @param req - A pointer to the whole SNMP SET request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void	        cleanup_set_request(Request*, int&) { }

	/**
	 * Serialize the value of the receiver.
	 * 
	 * @param buf - A pointer to byte stream buffer returned.
	 * @param sz - The size of the buffer returned.
	 * @return TRUE if serialization was successful, FALSE otherwise.
	 */
	virtual boolean      	serialize(char*&, int&) { return FALSE; }

	/**
	 * Read the value of the receiver from a byte stream.
	 * 
	 * @param buf 
	 *    a pointer to the input byte stream.
	 * @param sz
	 *    the size of the input buffer. On output it contains the
	 *    size remaining unread in the input buffer.
	 * @return 
	 *    TRUE if deserialization was successful, FALSE otherwise.
	 */
	virtual boolean      	deserialize(char*, int&) { return FALSE; }

	/**
	 * Check whether the receiver node contains any instance of a
	 * managed object.
	 *
	 * @return TRUE if the node currently manages an instance.
	 */  
	virtual boolean		is_empty() { return FALSE; }


	// communication between mib objects

	/**
	 * Return the immediate successor of the greatest object identifier 
	 * within the receiver's scope
	 * 
	 * @return 
	 *    a pointer to the non including upper bound of the receiver's 
	 *    scope.
	 */
	virtual OidxPtr      	max_key() { return &upperBound; }

protected:
	Oidx			upperBound;
};


/*------------------------ class MibStaticEntry ------------------------*/

/**
 * The MibStaticEntry class represents an entry (instance) within a
 * MibStaticTable.
 *
 * @author Frank Fock
 * @version 3.4 
 */ 

class AGENTPP_DECL MibStaticEntry: public Vbx {
 public:
	MibStaticEntry(const Vbx& v): Vbx(v) { }
	MibStaticEntry(const Oidx& o, const NS_SNMP SnmpSyntax& v): Vbx(o)
	  { set_value(v); } 
	MibStaticEntry(const MibStaticEntry& other): Vbx(other) { }

	OidxPtr		key() { return (Oidx*)&iv_vb_oid; }
};

/*------------------------ class MibStaticTable ------------------------*/

/**
 * The MibStaticTable class is a light weight table implementation for
 * static (read-only) tables and scalar groups. 
 * This class may be used as an example showing how 
 * the abstract class MibComplexEntry can be extended by subclassing
 * to implement tables that access their management information through 
 * a proprietary protocol (other than SNMP or AgentX).
 *
 * Caution: From version 3.5.2 on, MibStaticEntries added to a 
 * MibStaticTable should contain their respective OID suffix to
 * the table. The scalar group snmpGroup would thus be defined as 
 * follows:
 *
 * MibStaticTable* st = new MibStaticTable("1.3.6.1.2.1.11");
 * st->add(new MibStaticTable("1.0", Counter32(0))); // snmpInPkts
 * st->add(new MibStaticTable("2.0", Counter32(0))); // snmpOutPkts
 * st->add(new MibStaticTable("3.0", Counter32(0))); // snmpInBadVersions
 * 
 * The old format where the full OID had to be specified is still
 * support for compatibility reasons but should no longer be used,
 * because it might be removed in future versions!
 * 
 * @author Frank Fock
 * @version 3.5.2
 */
#if !defined (AGENTPP_DECL_TEMPL_OIDLIST_MIBSTATICENTRY)
#define AGENTPP_DECL_TEMPL_OIDLIST_MIBSTATICENTRY
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL OidList<MibStaticEntry>;
#endif

class AGENTPP_DECL MibStaticTable: public MibComplexEntry {
 public:
	/**
	 * Construct a static table (entry) instance with a given 
	 * object identifier. 
	 * 
	 * @param oid
	 *    an object identifier.
	 */   
	MibStaticTable(const Oidx&);

	/**
	 * Copy constructor.
	 * 
	 * @param other
	 *    another MibComplexEntry instance.
	 */
	MibStaticTable(MibStaticTable&);

	/**
	 * Destructor
	 */
	virtual ~MibStaticTable();

	/**
	 * Return a clone of the receiver.
	 *
	 * @return
	 *    a pointer to a clone of the MibStaticTable object.  
	 */
	virtual MibEntry*	clone() { return new MibStaticTable(*this); }

	/**
	 * Add an instance to the table. If such an instance already
	 * exists, it will be removed. (SYNCHRONIZED)
	 *
	 * @param instance
	 *    a MibStaticEntry instance.
	 */
	virtual void		add(const MibStaticEntry&);

	/**
	 * Remove an instance from the table. (SYNCHRONIZED)
	 *
	 * @param oid
	 *    the object ID of the entry to be removed.
	 */
	virtual void		remove(const Oidx&);

	/**
	 * Get the entry instance with the given OID. If suffixOnly 
	 * is FALSE (the default), the specified OID must be the full 
	 * OID of the entry, including the OID prefix from the 
	 * MibStaticTable. (NOT SYBCHRONIZED)
	 *
	 * @param oid
	 *    the OID (or OID suffix) of the requested entry.
	 * @param suffixOnly
	 *    determines whether the given OID should be interpreted
	 *    as suffix appended to the table's OID or whether the 
	 *    given OID fully specifies the requested entry.
	 * @return
	 *    the entry with the given OID or 0 if such an object does
	 *    not exist.
	 */
	virtual MibStaticEntry* get(const Oidx&, boolean suffixOnly=FALSE);

	/**
	 * Return the successor of a given object identifier within the 
	 * receiver's scope and the context of a given Request.
	 *
	 * @param oid
	 *    an object identifier
	 * @param request
	 *    a pointer to a Request instance.
	 * @return 
	 *    an object identifier if a successor could be found,
	 *    otherwise (if no successor exists or is out of scope) 
	 *    a zero length oid is returned
	 */
	virtual Oidx		find_succ(const Oidx&, Request* req = 0);

	/**
	 * Let the receiver process a SNMP GET subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GET request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void		get_request(Request*, int);

	/**
	 * Let the receiver process a SNMP GETNEXT subrequest
	 * 
	 * @param req - A pointer to the whole SNMP GETNEXT request.
	 * @param ind - The index of the subrequest to be processed.
	 */
	virtual void		get_next_request(Request*, int);
	
 protected:
	
	OidList<MibStaticEntry>		contents;
};
#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
