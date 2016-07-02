/*_############################################################################
  _## 
  _##  v3_mib.h  
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
#ifndef v3_mib_h_
#define v3_mib_h_

#include <agent_pp/agent++.h>

#ifdef _SNMPv3

#include <agent_pp/mib.h>
#include <snmp_pp/usm_v3.h>
#include <snmp_pp/v3.h>
#include <agent_pp/snmp_textual_conventions.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/**********************************************************************
 *  
 *  class V3SnmpEngine
 * 
 **********************************************************************/

class AGENTPP_DECL V3SnmpEngine: public MibGroup {

public:

	V3SnmpEngine(void);
};

class AGENTPP_DECL V3SnmpEngineID: public MibLeaf {

public:
       V3SnmpEngineID(const NS_SNMP v3MP *mp);
       void get_request(Request*, int);
private:
       const NS_SNMP v3MP *v3mp;
};

class AGENTPP_DECL V3SnmpEngineBoots: public MibLeaf {

public:
       V3SnmpEngineBoots(const NS_SNMP USM *u);
       void get_request(Request*, int);
private:
       const NS_SNMP USM *usm;
};

class AGENTPP_DECL V3SnmpEngineTime: public MibLeaf {

public:
       V3SnmpEngineTime(const NS_SNMP USM *u);
       void get_request(Request*, int);
private:
       const NS_SNMP USM *usm;
};

class AGENTPP_DECL V3SnmpEngineMaxMessageSize: public MibLeaf {

public:
       V3SnmpEngineMaxMessageSize();
};


class AGENTPP_DECL UsmUserTableStatus: public snmpRowStatus 
{
 public:
  UsmUserTableStatus(const Oidx&, int _base_len, NS_SNMP USM *usm);
  virtual ~UsmUserTableStatus();

  virtual MibEntryPtr clone();
  virtual int set(const Vbx& vb);
  virtual int unset();

  void deleteUsmUser();
  void addUsmUser();

 private:
  int base_len;
  NS_SNMP USM *usm;
};

class AGENTPP_DECL UsmUserTable: public StorageTable
{
 public:
  UsmUserTable();
  virtual ~UsmUserTable();

  virtual boolean ready_for_service(Vbx* pvbs, int sz);
  virtual void row_added(MibTableRow* new_row, const Oidx& ind, MibTable*);
  virtual void row_init(MibTableRow* new_row, const Oidx& ind, MibTable*);
  boolean addNewRow(const NS_SNMP OctetStr& engineID, const NS_SNMP OctetStr& userName,
                    const NS_SNMP OctetStr& securityName,
		    int authProtocol, const NS_SNMP OctetStr& authKey,
		    int privProtocol, const NS_SNMP OctetStr& privKey,
                    const boolean add_to_usm = TRUE);

  boolean addNewRow(const NS_SNMP OctetStr& engineID, const NS_SNMP OctetStr& userName,
		    int authProtocol, const NS_SNMP OctetStr& authKey,
		    int privProtocol, const NS_SNMP OctetStr& privKey,
                    const boolean add_to_usm = TRUE);

  boolean addNewRow(const NS_SNMP OctetStr& userName,
                    int authProtocol,
                    int privProtocol,
                    const NS_SNMP OctetStr& authPassword,
                    const NS_SNMP OctetStr& privPassword);
  boolean deleteRow(const NS_SNMP OctetStr& engineID,
                    const NS_SNMP OctetStr& userName);
  MibTableRow* get_row(Oidx o);

  static const Oidx auth_base;
  static const Oidx priv_base;

 protected:
  void initialize_key_change(MibTableRow*);

 private:
  NS_SNMP USM *usm;
};


class AGENTPP_DECL UsmCloneFrom: public MibLeaf
{
 public:
  UsmCloneFrom(Oidx o);
  virtual ~UsmCloneFrom() {};
  virtual int prepare_set_request(Request* req, int& ind);
  virtual void get_request(Request* req, int ind);
  virtual int set(const Vbx& vb);
  virtual boolean value_ok(const Vbx& vb);
  virtual MibEntryPtr clone();
 private:
  NS_SNMP USM *usm;
};

class AGENTPP_DECL UsmKeyChange: public MibLeaf
{
 public:
  UsmKeyChange(Oidx o, int keylen, int hashfunction, int typeOfKey,
	       UsmKeyChange* ukc, NS_SNMP USM *u);
  UsmKeyChange(Oidx o, NS_SNMP USM *u);
  virtual ~UsmKeyChange();
  
  virtual int unset();
  void initialize(int keylen, int hashfunction, int typeOfKey, UsmKeyChange* ukc);
  virtual void get_request(Request* req, int ind);
  virtual int prepare_set_request(Request* req, int& ind);
  virtual int set(const Vbx& vb);
  virtual boolean value_ok(const Vbx& vb);
  virtual MibEntryPtr clone();

 protected:
  boolean process_key_change(NS_SNMP OctetStr& os);

  int type_of_key;
  int key_len;
  int hash_function;
  UsmKeyChange* otherKeyChangeObject;
  NS_SNMP USM *usm;
};

class AGENTPP_DECL UsmOwnKeyChange: public UsmKeyChange
{
 public:
  UsmOwnKeyChange(Oidx o, NS_SNMP USM *u) : UsmKeyChange(o, u) {};
  UsmOwnKeyChange(Oidx o, int keylen, int hashfunction, int typeOfKey,
		  UsmKeyChange* ukc, NS_SNMP USM *u)
    : UsmKeyChange(o, keylen, hashfunction, typeOfKey, ukc, u){};
  virtual ~UsmOwnKeyChange();
  
  virtual int prepare_set_request(Request* req, int& ind);
  virtual MibEntryPtr clone();

 private:

};


/**********************************************************************
 *  
 *  class UsmStatsUnsupportedSecLevels
 * 
 **********************************************************************/

class AGENTPP_DECL UsmStatsUnsupportedSecLevels: public MibLeaf {

public:
	UsmStatsUnsupportedSecLevels(const NS_SNMP USM *u);
        void get_request(Request*, int);
private:
	const NS_SNMP USM *usm;
};

/**********************************************************************
 *  
 *  class UsmStatsNotInTimeWindows
 * 
 **********************************************************************/

class AGENTPP_DECL UsmStatsNotInTimeWindows: public MibLeaf {

public:
	UsmStatsNotInTimeWindows(const NS_SNMP USM *u);
        void get_request(Request*, int);
private:
	const NS_SNMP USM *usm;
};

/**********************************************************************
 *  
 *  class UsmStatsUnknownUserNames
 * 
 **********************************************************************/

class AGENTPP_DECL UsmStatsUnknownUserNames: public MibLeaf {

public:
	UsmStatsUnknownUserNames(const NS_SNMP USM *u);
        void get_request(Request*, int);
private:
	const NS_SNMP USM *usm;
};

/**********************************************************************
 *  
 *  class UsmStatsUnknownEngineIDs
 * 
 **********************************************************************/

class AGENTPP_DECL UsmStatsUnknownEngineIDs: public MibLeaf {

public:
	UsmStatsUnknownEngineIDs(const NS_SNMP USM *u);
        void get_request(Request*, int);
private:
	const NS_SNMP USM *usm;
};

/**********************************************************************
 *  
 *  class UsmStatsWrongDigests
 * 
 **********************************************************************/

class AGENTPP_DECL UsmStatsWrongDigests: public MibLeaf {

public:
	UsmStatsWrongDigests(const NS_SNMP USM *u);
        void get_request(Request*, int);
private:
	const NS_SNMP USM *usm;
};

/**********************************************************************
 *  
 *  class UsmStatsDecryptionErrors
 * 
 **********************************************************************/

class AGENTPP_DECL UsmStatsDecryptionErrors: public MibLeaf {

public:
	UsmStatsDecryptionErrors(const NS_SNMP USM *u);
        void get_request(Request*, int);
private:
	const NS_SNMP USM *usm;
};


/**********************************************************************
 *  
 *  class UsmStats
 * 
 **********************************************************************/

class AGENTPP_DECL UsmStats: public MibGroup {

public:

	UsmStats(void);
};

class	AGENTPP_DECL usm_mib: public MibGroup {

public:

        usm_mib(UsmUserTable*);
};


class AGENTPP_DECL MPDGroup: public MibGroup {

public:

       MPDGroup(void);
};

class AGENTPP_DECL MPDGroupSnmpUnknownSecurityModels: public MibLeaf {

public:
	MPDGroupSnmpUnknownSecurityModels(const NS_SNMP v3MP *mp);
        void get_request(Request*, int);
private:
	const NS_SNMP v3MP *v3mp;
};

class AGENTPP_DECL MPDGroupSnmpInvalidMsgs: public MibLeaf {

public:
	MPDGroupSnmpInvalidMsgs(const NS_SNMP v3MP *mp);
        void get_request(Request*, int);
private:
	const NS_SNMP v3MP *v3mp;
};

class AGENTPP_DECL MPDGroupSnmpUnknownPDUHandlers: public MibLeaf {

public:
	MPDGroupSnmpUnknownPDUHandlers(const NS_SNMP v3MP *mp);
        void get_request(Request*, int);
private:
	const NS_SNMP v3MP *v3mp;
};


class AGENTPP_DECL usmUserAuthProtocol: public MibLeaf {

 public:
	usmUserAuthProtocol(const Oidx&, NS_SNMP USM *u);
	virtual boolean value_ok(const Vbx&);
	virtual MibEntryPtr clone();
 private:
	NS_SNMP USM *usm;
};

class AGENTPP_DECL usmUserPrivProtocol: public MibLeaf {

 public:
	usmUserPrivProtocol(const Oidx&, NS_SNMP USM *u);
	virtual boolean value_ok(const Vbx&);
	virtual int prepare_set_request(Request*, int&);
	virtual MibEntryPtr clone();
 private:
	NS_SNMP USM *usm;
};
#ifdef AGENTPP_NAMESPACE
}
#endif
#endif

#endif

