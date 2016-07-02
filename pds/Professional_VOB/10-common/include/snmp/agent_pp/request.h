/*_############################################################################
  _## 
  _##  request.h  
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

#ifndef request_h_
#define request_h_

#include <agent_pp/agent++.h>
#include <agent_pp/snmp_pp_ext.h>
#include <agent_pp/List.h>
#include <agent_pp/threads.h>

#ifdef _SNMPv3
// vacm.cpp needs vacm.h and vacm_class.h
#include <agent_pp/vacm_class.h>
#endif

#ifndef DEFAULT_WRITE_COMMUNITY
#define DEFAULT_WRITE_COMMUNITY	"private"
#endif
#ifndef DEFAULT_READ_COMMUNITY
#define DEFAULT_READ_COMMUNITY	"public"
#endif

#define PHASE_DEFAULT		0
#define PHASE_PREPARE		1
#define PHASE_COMMIT		2
#define PHASE_UNDO		3
#define PHASE_CLEANUP		4

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/*--------------------------- class Request --------------------------*/

/**
 * The Request class represents SNMP requests. A Request contains a
 * PDU, a from address (UDP), an unique transaction id, a community 
 * string, the SNMP version, and two arrays that hold for each variable
 * binding of the PDU flags which determine whether the variable
 * binding has been processed or not, and if it is ready or not (used
 * by MibEntry::prepare_set_request). For SNMPv3 a Request contains 
 * two additional attributes, that are the viewName that applies for
 * the request and a pointer the corresponding Vacm.
 *
 * SET requests are processed through four phases:
 * PHASE_PREPARE(p), PHASE_COMMIT(c), PHASE_UNDO(u), and PHASE_CLEANUP(f).
 * There are three possible sequences that may be followed for a
 * particular SET transaction: 
 * 1. (p), (c), (f)
 * 2. (p), (c), (u)
 * 3. (p),      (f)
 *
 * @author Frank Fock
 * @version 3.5.23
 */

#if !defined (AGENTPP_DECL_TEMPL_ARRAY_MIBENTRY)
#define AGENTPP_DECL_TEMPL_ARRAY_MIBENTRY
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL Array<MibEntry>;
#endif

class AGENTPP_DECL Request: public Synchronized {
friend class RequestList;
friend class Mib;
public:

#ifdef _SNMPv3
	/**
	 * Construct a Request (only available with _SNMPv3).
	 *
	 * @param pdu
	 *    a Pdux instance.
	 * @param fromAddress
	 *    the UTarget containg the originator's address.
	 */
	Request(const Pdux&, const NS_SNMP UTarget&);
#else
	/**
	 * Construct a Request (only available without _SNMPv3).
	 *
	 * @param pdu
	 *    a Pdux instance.
	 * @param fromAddress
	 *    the CTarget containg the originator's address.
	 */
        Request(const Pdux&, const NS_SNMP CTarget&);
#endif

	/**
	 * Copy constructor.
	 *
	 * @param other
	 *    another Request instance.
	 */
	Request(const Request&);

	/**
	 * Destructor.
	 */
	virtual ~Request();

	/**
	 * Check whether the receiver contains a specified variable binding.
	 * @note Two variable bindings are supposed to be the same, 
	 * if their oids are equal.
	 * 
	 * @param vb - A variable binding.
	 */
	virtual boolean		contains(const Vbx&);

	/**
	 * Return the position of a specified variable binding in the 
	 * receiver request.
	 * @note Two variable bindings are supposed to be the same, 
	 * if their oids are equal.
	 * 
	 * @param vb - A variable binding.
	 */
	virtual int		position(const Vbx&);

	/**
	 * Check whether the receiver request is finished (all variable 
	 * bindings have been processed).
	 *
	 * @return TRUE if the request is complete, FALSE otherwise.
	 */ 
        virtual boolean 	finished() const;

	/**
	 * Check whether a specified variable binding (sub-request) 
	 * has been processed.
	 *
	 * @param i - The index (starting from 0) of the variable binding
	 *            to check.
	 * @return TRUE if the sub-request is done, FALSE otherwise.
	 */   
	virtual boolean		is_done(int) const; 

	/**
	 * Finish a sub-request by setting the receiver´s variable binding
	 * at the given sub-request index to a specified value and marking
	 * the sub-request as done.
	 *
	 * @param i - The index of the sub-request to finish.
	 * @param vb - A variable binding.
	 */
	virtual void    	finish(int, const Vbx&);

	/**
	 * Mark a subrequest as done, without changing its value.
	 *
	 * @param index
	 *    the index of the sub-request to finish.
	 */
	virtual void    	finish(int);

	/**
	 * Set a sub-request ready for commit.
	 *
	 * @param i - The index of the sub-request to be set ready for commit.
	 */
	virtual void    	set_ready(int);

	/**
	 * Set a sub-request not-ready for commit.
	 *
	 * @param i - The index of the sub-request to be set not-ready for 
	 *            commit.
	 */
	virtual void    	unset_ready(int);

	/**
	 * Check whether a specified variable binding (sub-request) 
	 * is ready to commit (applies only for SET-Requests).
	 *
	 * @param i - The index (starting from 0) of the variable binding
	 *            to check.
	 * @return TRUE if the sub-request is ready, FALSE otherwise.
	 */   
	virtual boolean		is_ready(int) const;
	
	/**
	 * Return the index of the first pending (done == FALSE) subrequest.
	 *
	 * @return
	 *    the index of the first non ready subrequest or -1 if request
	 *    is finished.
	 */
	virtual int		first_pending() const;	

	/**
	 * Set the error status of the receiver request.
	 *
	 * @param index - The index of the sub-request that failed.
	 * @param error - An error code describing the error condition.
	 */ 
	virtual void    	error(int, int);

#ifdef _SNMPv3
	/**
	 * Set the VACM error status of the receiver request.
	 *
	 * @param index - The index of the sub-request that failed.
	 * @param error - An error code describing the error condition.
	 */ 
        virtual void            vacmError(int, int); 
#endif

	/**
	 * Return the variable binding of the specified sub-request.
	 *
	 * @param index - An index of a sub-request (starting from 0).
	 * @return A variable binding.
	 */ 
	virtual Vbx     	get_value(int);

        /**
	 * Get the value for a given OID if the request contains such
	 * an instance. 
	 *
	 * @param oid
	 *    an Oidx instance denoting the OID of the variable binding
	 *    to return.
	 * @return
	 *    a pointer to a clone of the first Vbx instance in the 
	 *    request with the specified OID, or 0 if no such instance 
	 *    exists. 
	 */
	Vbx*			search_value(const Oidx&) const;

	/**
	 * Gets the original variable binding of the specified sub-request.
	 *
	 * @param index
	 *    the zero-based index of the sub-request.
	 * @return
	 *    a variable binding.
	 */
	Vbx			get_original_value(unsigned int i)
					{ return originalVbs[i]; }

	/**
	 * Return the syntax of the specified sub-request (variable binding).
	 *
	 * @param index - An index of a sub-request (starting from 0).
	 * @return A SMI syntax.
	 */ 
	virtual NS_SNMP SnmpInt32	get_syntax(int);

	/**
	 * Return the object identifier of the specified
	 * sub-request.
	 *
	 * @param index - An index of a sub-request (starting from 0). 
	 * @return An object identifier.
	 */
	virtual Oidx		get_oid(int);		

	/**
	 * Set the object identifier of a specified sub-request.
	 *
	 * @param oid - An object identifier.
	 * @param index - An index of a sub-request (starting from 0).
	 */  
	virtual void		set_oid(const Oidx&, int);		


	/**
	 * Return the request type.
	 *
	 * @return The PDU type of the receiver.
	 */
	unsigned short 	get_type()		{ return pdu->get_type(); }

	/**
	 * Return the request id of the receiver.
	 *
	 * @return A request id.
	 */ 
	unsigned long	get_request_id()   { return pdu->get_request_id(); }

	/**
	 * Return the agent wide unique transaction id of the receiver.
	 *
	 * @return A transaction id.
	 */
	unsigned long	get_transaction_id()   { return transaction_id; }

	/**
	 * Return the error status of the receiver request.
	 *
	 * @return A SNMP error status.
	 */ 
	int		get_error_status() { return pdu->get_error_status(); }

	/**
	 * Return the index of the sub-request that failed with an error
	 * condition.
	 * 
	 * @return The index of the failed sub-request (starting from 1).
	 */
	int		get_error_index()  { return pdu->get_error_index(); }

	/**
	 * Set the error status of the receiver request.
	 * 
	 * @param s - An error status.
	 */ 
	void		set_error_status(int s) { pdu->set_error_status(s); }

	/**
	 * Set the error index of the receiver request.
	 * 
	 * @param s - The sub-request index of a failed sub-request.
	 * @note All error indexes starts at 1.
	 */ 
	void		set_error_index(int i)  { pdu->set_error_index(i); }

	/**
	 * Set the transaction id of the receiver sub-request.
	 *
	 * @param id - An unique transaction id.
	 */
	void		set_transaction_id(unsigned long id) 
						{ transaction_id = id; }

	/**
	 * Return the count of sub-requests in the receiver request.
	 *
	 * @return The request size.
	 */
	int		subrequests() { return size; }


	/**
	 * Return the count of non repetition variable bindings of a 
	 * GETBULK request.
	 *
	 * @return The number of non-repetition variable bindings.
	 */
	int		get_non_rep() { return non_rep; }

	/**
	 * Return the count of repetition variable bindings of a 
	 * GETBULK request.
	 *
	 * @return The number of non-repetition variable bindings.
	 */
	int		get_rep() { return repeater; }

	/**
	 * Return the maximum repetition of repetition variable bindings 
	 * of a GETBULK request.
	 *
	 * @return The number of non-repetition variable bindings.
	 */
	int		get_max_rep() { return max_rep; }


	/**
	 * Add a repetition row to the GETBULK request PDU.
	 *
	 * @return TRUE if there was enough room in the response PDU for
	 *         another repetition, FALSE otherwise.
	 */ 
	virtual boolean		add_rep_row();

	/**
	 * Initialize a repetition row of a GETBULK request PDU.
	 *
	 * @param row
	 *    the repetition row to be initialized (counted from 1, row 0
	 *    is the non-repeated first row) 
	 * @return 
	 *    TRUE if such a row exists, FALSE otherwise.
	 */ 
	virtual boolean		init_rep_row(int);

	/**
	 * Trim the request's variable bindings to a given number
	 * of elements. Elements are removed starting from the end.
	 *
	 * @param count
	 *    the count of variable bindings that should remain. 
	 */ 
	virtual void		trim_request(int);

	/**
	 * Increment the number of variable bindings to be processed by one.
	 */
	void			inc_outstanding();
	/**
	 * Decrement the number of variable bindings to be processed by one
	 */
	void			dec_outstanding();

	/**
	 * Set the number of variable bindings to be processed to 0.
	 */
	void			no_outstanding();

	/**
	 * Return a pointer to the receiver's PDU.
	 *
	 * @return
	 *    a pointer to a Pdux instance.
	 */
	Pdux*		get_pdu() { return pdu; }

#ifdef _SNMPv3
        /**
         * Return the security_name/community string of the receiver request.
         *
         * @param s - An OctetStr to hold the returned security_name 
         *            Note: the SNMPv1/v2c community is mapped to 
         *                  the security_name.
         */ 
        void		get_security_name(NS_SNMP OctetStr& s);

	/**
	 * Return the security model used for the request.
	 *
	 * @return
	 *    a security model (1 == SNMPv1, 2==SNMPv2c, 3=USM).
	 */
	unsigned int   	get_security_model() { return version; }

	/**
	 * Initialize the VACM and viewName of the receiver request.
	 *
	 * @param v - A pointer to a Vacm.
	 * @param vname - The viewName that applies for the receiver.
	 */ 
        void            init_vacm(Vacm*, const NS_SNMP OctetStr&); 

	/**
	 * Get the view name.
	 *
	 * @return
	 *    an OctetStr instance containing the view name associated 
	 *    with the receiver.
	 */
	NS_SNMP OctetStr	get_view_name() const { return viewName; }

	/**
	 * Get context.
	 *
	 * @return
	 *    the request's context.
	 */
	NS_SNMP OctetStr	get_context() const { return pdu->get_context_name(); }

	/**
	 * Return a pointer to the request's originator address.
	 * (only available with _SNMPv3)
	 *
	 * @return
	 *    a pointer to a UTarget instance.
	 */
	NS_SNMP UTarget*       	get_address() { return &target; }

#else

	/**
	 * Return a pointer to the request's originator address.
	 * (only available without _SNMPv3)
	 *
	 * @return
	 *    a pointer to a CTarget instance.
	 */
	NS_SNMP CTarget*       	get_address() { return &target; }
#endif

	/**
	 * Get the request's version.
	 *
	 * @return
	 *    the SNMP version of the request.
	 */
	NS_SNMP snmp_version	get_snmp_version() { return version; }

	u_char		phase;

	/**
	 * Get a pointer to the MibEntry object that has been locked
	 * for the given subrequest.
	 *
	 * @param ind
	 *    a subrequest index.
	 * @return
	 *    0 if such an object has not been locked or has been already 
	 *    released, otherwise a pointer to a MibEntry instance is 
	 *    returned that has been locked for the current request.
	 */
	MibEntry*	get_locked(int);

	/**
	 * Set the locked MibEntry for the given subrequest id.
	 *
	 * @param ind
	 *    a subrequest index.
	 * @param entry
	 *    a pointer to a MibEntry instance or 0 to indicate
	 *    that the specified subrequest no longer holds a lock. 
	 */
	void		set_locked(int, MibEntry*); 

	/**
	 * Set the given subrequest as unlocked.
	 *
	 * @param ind
	 *    a subrequest index.
	 */
	void		set_unlocked(int); 

	/**
	 * Determine the lock index of a MibEntry. The lock index is the
	 * index of the subrequest that acquired a lock for the given
	 * MibEntry.
	 *
	 * @param entry
	 *	a pointer to a MibEntry object.
	 * @return
	 *	-1 if entry has not been locked by this request (or if
	 *      such a lock has already been released). Otherwise, it
	 *      returns the smallest subrequest index that locked entry.
	 */
	int		lock_index(MibEntry*);

	/**
	 * Trim BULK response PDU to N+M*R variables and set OIDs of variables
	 * with END OF MIB VIEW syntax to either the corresponding request 
	 * OID (R == 0) or to the last successor found. N = nonrep, M = maxrep,
	 * R = rep.
	 *
	 * This method does is a NOP when called on a request type other than 
	 * GETBULK.
	 */
	void            trim_bulk_response();

#ifdef NO_FAST_MUTEXES
	/**
	 * Initialize the lock queue if not already initialized.
	 */
	void		init_lock_queue();    

	/**
	 * Frees any resources used by the lock queue. This method must not
	 * be called while requests are being processed.
	 */
	void		delete_lock_queue() 
	    { if (lockQueue) { delete lockQueue; lockQueue = 0; } }
#endif

protected:

	Request();

	virtual void    init_from_pdu();

	void		check_exception(int, Vbx&);

	int	  	get_max_response_length(); 

	Pdux*		pdu;
	Vbx*		originalVbs;
	int		originalSize;

	NS_SNMP UdpAddress     	from;
	boolean*       	done;
	boolean*	ready;
	int		outstanding;
	int		size;

	int		non_rep;
	int		max_rep;
	int		repeater;

	NS_SNMP snmp_version	version;
	unsigned long	transaction_id;

	// Locks hold by a mulit-phase (SET) request
	Array<MibEntry>	locks;

#ifdef _SNMPv3
	NS_SNMP OctetStr	viewName;
        Vacm*           vacm;
        NS_SNMP UTarget         target;
#else
	NS_SNMP CTarget		target;
#endif
#ifdef NO_FAST_MUTEXES
	static LockQueue* lockQueue;
#endif
};


/*------------------------ class RequestList --------------------------*/

/**
 * The RequestList represents a non ordered queue of Requests. 
 * On multi-threaded SNMP agents this queue could contain more than 
 * one pending Request at once. The RequestList provides member functions 
 * to receive, process, lookup, and modify Requests.
 *
 * The following example illustrates, how RequestList may be used in an
 * v1 or v2c SNMP agent:
 * 
 * main (int argc, char* argv[])
 * {
 *         int status;
 *         Snmpx snmp(status, 161);  // create SNMP++ session
 * 
 *         if (status != SNMP_CLASS_SUCCESS) {      // if fail print error
 *                 cout << snmp.error_msg(status);  // message
 *                 exit(1);
 *         }
 * 
 *         Mib mib;                  // create the agents MIB 
 *         RequestList reqList;
 *
 *         mib.set_request_list(&reqList); // register RequestList
 *         // to be used by mib for outgoing requests
 * 
 *         reqList.set_snmp(&snmp);   // register the Snmpx object to
 *         // be used by the RequestList for incoming SNMP requests 
 *
 *         mib.add(new systemGroup());  // add sysGroup and snmpGroup 
 *         mib.add(new snmpGroup());    // (provided with AGENT++) to mib
 *         ...
 * 
 *         Request* req;             // pointer to an incoming SNMP request
 *         for (;;) {                // loop forever (agent is a daemon)
 * 	  
 *                 // wait for incoming request max 120 sec
 *                 // and then just loop once
 *                 req = reqList.receive(120);
 * 
 *                 if (req) mib.process_request(req); // process the request
 *         }
 * }
 *
 * @author Frank Fock
 * @version 3.5.14
 */

class AGENTPP_DECL RequestList: public ThreadManager {
public:
	/**
	 * Default constructor
	 */
	RequestList();

	/**
	 * Destructor
	 * (SYNCHRONIZED)
	 */
	virtual ~RequestList();

	/**
	 * Activate or deactivate source address
	 * validation for SNMP v1/v2c requests (SNMP-COMMUNITY-MIB).
	 *
	 * @param sourceAddressFiltering
	 *    if TRUE incoming v1/v2c requests will be validated 
	 *    (not only) on the transport address including the UDP
	 *    port.
	 */
	virtual void		set_address_validation(boolean);
	
	/**
	 * Gets the current state of the source address validation for
	 * community base SNMP versions.
	 *
	 * @return 
	 *    TRUE if source address validation is activated.
	 */
	boolean			get_address_validation() 
	    { return sourceAddressValidation; }

	/**
	 * Set the Snmpx session to be used for incoming requests.
	 *
	 * @param session - A pointer to Snmpx instance.
	 */
	virtual void		set_snmp(Snmpx* session) { snmp = session; }

	/**
	 * Get the Snmpx session used by this RequestList.
	 *
	 * @return
	 *    a Snmpx instance.
	 */
	Snmpx*			get_snmp() { return snmp; }

#ifdef _SNMPv3
	/**
	 * Set v3MP to be used.
	 *
	 * @param mp - A pointer to a v3MP instance.
	 */
        virtual void             set_v3mp(NS_SNMP v3MP* mp) { v3mp = mp; }


	/**
	 * Set VACM to be used.
	 *
	 * @param v - A pointer to a Vacm instance.
	 */
        virtual void             set_vacm(Vacm* v) { vacm = v; }

	/**
	 * Return the a pointer to the VACM used by the receiver.
	 *
	 * @return 
	 *    a pointer to the VACM used by the receiver.
	 */
	virtual Vacm*		get_vacm() { return vacm; }

	/**
	 * Return the a pointer to the v3MP used by the receiver.
	 *
	 * @return 
	 *    a pointer to the v3MP used by the receiver.
	 */
	virtual NS_SNMP v3MP*		get_v3mp() { return v3mp; }
#else
	/**
	 * Set READ community.
	 *
	 * @param readCommunity
	 *    the new community for all GET type requests.
	 */
	void			set_read_community(const NS_SNMP OctetStr&);

	/**
	 * Set WRITE community.
	 *
	 * @param writeCommunity
	 *    the new community for SET requests.
	 */
	void			set_write_community(const NS_SNMP OctetStr&);

#endif
	/**
	 * Wait a given time for an incoming request.
	 *
	 * @param timeout
	 *    The maximum time in seconds to wait for an incoming request.
	 *    If timeout is 0 receive will not block, if it is <0 receive
	 *    will wait forever.
	 * @return A pointer to the received request or 0 if within the 
	 *         timeout period no request has been received.
	 */
	virtual Request*		receive(int);

	/** 
	 * Return the corresponding request id of a request in the
	 * receiver RequestList that contains the specified variable
	 * binding.
	 * (SYNCHRONIZED)
	 *
	 * @param vb - A variable binding.
	 * @return A request id or 0 if a corresponding request could
	 *         not have been found.
	 */ 
	virtual unsigned long	get_request_id(const Vbx&);

	/**
	 * Mark a sub-request as done and put the result of the 
	 * sub-request into the response PDU. 
	 * (SYNCHRONIZED)
	 *
	 * @param request_id
	 *    a request id of a pending request. 
	 * @param index
	 *    the index of the target sub-request.
	 * @param vb
	 *    a variable binding containing the result of an sub-request.
	 * @return 
	 *    TRUE if request is ready to answer, FALSE otherwise.
	 */
	virtual boolean		done(unsigned long, int, const Vbx&);

	/**
	 * Mark a sub-request as done with an error and put the corresponding
	 * SNMP error code for the sub-request into the response PDU. 
	 * (SYNCHRONIZED)
	 *
	 * @param request_id
	 *    a request id of a pending request. 
	 * @param index
	 *    the index of a failed sub-request.
	 * @param err
	 *    an SNMP error status. 
	 */
	virtual void		error(unsigned long, int, int);

	/**
	 * Return a pointer to the request identified by a given 
	 * transaction id. (SYNCHRONIZED)
	 *
	 * @param request_id
	 *    a transaction id.
	 * @return 
	 *    a pointer to a Request or 0 if there is no request pending
	 *    with the given transaction id.
	 */
	virtual Request*		get_request(unsigned long);

	/**
	 * Return a pointer to the request identified by a given request id.
	 * (NOT SYNCHRONIZED)
	 *
	 * @param request_id - A request id.
	 * @return A pointer to a Request or 0 if there is no request pending
	 *         with the given request id.
	 */
	virtual Request*		find_request_on_id(unsigned long);

	/**
	 * Answer a Request by sending the corresponding response PDU.
	 * (SYNCHRONIZED)
	 *
	 * @param req - A pointer to a processed Request.
	 */
	virtual void			answer(Request*);

#ifdef _SNMPv3
	/**
	 * Send a report to the originator of the request. The PDU
	 * must contain the one variable binding with the actual value
	 * of the error counter specifying the report.
	 *
	 * @param req
	 *    a Request instance. 
	 */
	virtual void			report(Request*);
#endif

	/**
	 * Get next transaction id.
	 *
	 * @return
	 *    a unique transaction id.
	 */
	unsigned long			create_transaction_id() 
					{ return next_transaction_id++; }

	/**
	 * Check whether the request list (queue) is empty or not.
	 *
	 * @return
	 *    TRUE if the queue is empty.
	 */
	boolean				is_empty() { return requests->empty();}

	/**
	 * Get the actual size of the request list.
	 *
	 * @return
	 *    the size of the queue.
	 */
	unsigned long			size() { return requests->size(); }

protected:
	/**
	 * Add a Request to the RequestList.
	 * (SYNCHRONIZED)
	 *
	 * @param req - A pointer to a Request.
	 * @return A pointer to the added Request.
	 */
	virtual Request*		add_request(Request*);

	/**
	 * Check whether a given community is acceptable for the specified
	 * PDU type.
	 *
	 * @param pdutype - A PDU type (e.g., sNMP_PDU_SET, 
	 *                  sNMP_PDU_GET, etc.)
	 * @param community - A v1 or v2c community string.
	 * @return TRUE if the given community is ok, FALSE otherwise.
	 */ 
	virtual boolean		community_ok(int, const NS_SNMP OctetStr&);

	/**
	 * Sends an authenticationFailure notification if the corresponding
	 * MIB object is enabled.
	 *
	 * @param context
	 *    in which the failure occured. In most cases this is the 
	 *    default context ("").
	 * @param sourceAddress
	 *    the source address of the host that sent the unauthenticated
	 *    SNMP message. 
	 * @param status
	 *    the error code that further classifies the authentication
	 *    failure.
	 */
	virtual void    authenticationFailure(const NS_SNMP OctetStr& context,
					      const NS_SNMP GenAddress& sourceAddress,
					      int status);

	/**
	 * Set all variable bindings (for response) to null values.
	 * @param req
	 *    a pointer to a Request instance.
	 */
	static void	null_vbs(Request* req);
	
        List<Request>*		requests;
	Snmpx*			snmp;
#ifdef _SNMPv3
        Vacm*			vacm;
	NS_SNMP v3MP*                   v3mp;
#endif
	NS_SNMP OctetStr*		write_community;		
	NS_SNMP OctetStr*		read_community;		

	unsigned long		next_transaction_id;

	boolean			sourceAddressValidation;
};

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif
