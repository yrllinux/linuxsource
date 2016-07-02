/*_############################################################################
  _## 
  _##  vacm.h  
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
#ifndef vacm_h_
#define vacm_h_

#include <agent_pp/vacm_class.h>
#include <agent_pp/mib.h>
#include <agent_pp/snmp_textual_conventions.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


#define oidSnmpModules                    "1.3.6.1.6.3"
#define oidSnmpVacmMIB                    "1.3.6.1.6.3.16"
#define oidVacmMIBObjects                 "1.3.6.1.6.3.16.1"
#define oidVacmMIBConformance             "1.3.6.1.6.3.16.2"


#define oidVacmContextTable               "1.3.6.1.6.3.16.1.1"
#define oidVacmContextEntry               "1.3.6.1.6.3.16.1.1.1"
#define oidVacmContextName                "1.3.6.1.6.3.16.1.1.1.1"

#define oidVacmSecurityToGroupTable       "1.3.6.1.6.3.16.1.2"
#define oidVacmSecurityToGroupEntry       "1.3.6.1.6.3.16.1.2.1"
#define oidVacmSecurityModel              "1.3.6.1.6.3.16.1.2.1.1" // 1.index
#define oidVacmSecurityName               "1.3.6.1.6.3.16.1.2.1.2" // 2.index
#define oidVacmGroupName                  "1.3.6.1.6.3.16.1.2.1.3"
#define oidVacmSecurityToGroupStorageType "1.3.6.1.6.3.16.1.2.1.4"
#define oidVacmSecurityToGroupStatus      "1.3.6.1.6.3.16.1.2.1.5"


#define oidVacmAccessTable                "1.3.6.1.6.3.16.1.4"
#define oidVacmAccessEntry                "1.3.6.1.6.3.16.1.4.1"   // groupName 1. index
#define oidVacmAccessContextPrefix        "1.3.6.1.6.3.16.1.4.1.1" // 2. index
#define oidVacmAccessSecurityModel        "1.3.6.1.6.3.16.1.4.1.2" // 3. index
#define oidVacmAccessSecurityLevel        "1.3.6.1.6.3.16.1.4.1.3" // 4. index
#define oidVacmAccessContextMatch         "1.3.6.1.6.3.16.1.4.1.4"
#define oidVacmAccessReadViewName         "1.3.6.1.6.3.16.1.4.1.5"
#define oidVacmAccessWriteViewName        "1.3.6.1.6.3.16.1.4.1.6"
#define oidVacmAccessNotifyViewName       "1.3.6.1.6.3.16.1.4.1.7"
#define oidVacmAccessStorageType          "1.3.6.1.6.3.16.1.4.1.8"
#define oidVacmAccessStatus               "1.3.6.1.6.3.16.1.4.1.9"

#define oidVacmMIBViews                   "1.3.6.1.6.3.16.1.5"
#define oidVacmViewSpinLock               "1.3.6.1.6.3.16.1.5.1"
#define oidVacmViewTreeFamilyTable        "1.3.6.1.6.3.16.1.5.2"
#define oidVacmViewTreeFamilyEntry        "1.3.6.1.6.3.16.1.5.2.1"
#define oidVacmViewTreeFamilyViewName     "1.3.6.1.6.3.16.1.5.2.1.1" // 1. index
#define oidVacmViewTreeFamilySubtree      "1.3.6.1.6.3.16.1.5.2.1.2" // 2. index
#define oidVacmViewTreeFamilyMask         "1.3.6.1.6.3.16.1.5.2.1.3"
#define oidVacmViewTreeFamilyType         "1.3.6.1.6.3.16.1.5.2.1.4"
#define oidVacmViewTreeFamilyStorageType  "1.3.6.1.6.3.16.1.5.2.1.5"
#define oidVacmViewTreeFamilyStatus       "1.3.6.1.6.3.16.1.5.2.1.6"

#define oidVacmMIBCompliances             "1.3.6.1.6.3.16.2.1"
#define oidVacmMIBGroups                  "1.3.6.1.6.3.16.2.2"

// also defined in snmp++/include/v3.h
#ifndef oidSnmpUnavailableContexts
#define oidSnmpUnavailableContexts        "1.3.6.1.6.3.12.1.4.0"
#define oidSnmpUnknownContexts            "1.3.6.1.6.3.12.1.5.0"
#endif

#define VACM_accessAllowed          0
#define VACM_notInView              1
#define VACM_noSuchView             2
#define VACM_noSuchContext          3
#define VACM_noGroupName            4
#define VACM_noAccessEntry          5
#define VACM_otherError             6



class AGENTPP_DECL SnmpUnavailableContexts: public MibLeaf {

 public:
  SnmpUnavailableContexts();
  void incValue();
  NS_SNMP SnmpInt32 getValue();
};


class AGENTPP_DECL SnmpUnknownContexts: public MibLeaf {

 public:
  SnmpUnknownContexts();
  void incValue();
  NS_SNMP SnmpInt32 getValue();
};


/*********************************************************************

               VacmContextTable

 ********************************************************************/
class AGENTPP_DECL VacmContextTable: public MibTable {

public:
  VacmContextTable();
  ~VacmContextTable();

  boolean	isContextSupported(const NS_SNMP OctetStr& context);
  boolean	addNewRow(const NS_SNMP OctetStr& context);
  void          deleteRow(const NS_SNMP OctetStr& context);
};

/*********************************************************************

               VacmSecurityToGroupTable

 ********************************************************************/
class AGENTPP_DECL VacmSecurityToGroupTable: public StorageTable {

public:
  VacmSecurityToGroupTable();
  virtual ~VacmSecurityToGroupTable();

  virtual boolean	ready_for_service(Vbx*, int);
  virtual boolean	could_ever_be_managed(const Oidx&, int&);
  virtual void		row_added(MibTableRow*, const Oidx&, MibTable*);
  boolean		getGroupName(const int& securiyModel, 
				     const NS_SNMP OctetStr& securityName, 
				     NS_SNMP OctetStr& groupName);
  boolean		isGroupNameOK(const NS_SNMP OctetStr&);
  boolean		addNewRow(int securityModel, 
				  const NS_SNMP OctetStr& securityName, 
				  const NS_SNMP OctetStr& groupName, int storageType);
  void			deleteRow(int securityModel, 
				  const NS_SNMP OctetStr& securityName);
};


/*********************************************************************

               VacmAccessTable

 ********************************************************************/
class AGENTPP_DECL VacmAccessTable: public StorageTable {

public:
  VacmAccessTable(VacmSecurityToGroupTable*);
  virtual ~VacmAccessTable();

  virtual boolean	ready_for_service(Vbx*, int);
  virtual boolean	could_ever_be_managed(const Oidx&, int&);
  virtual void		row_added(MibTableRow*, const Oidx&, MibTable*);
  boolean		getViewName(const NS_SNMP OctetStr&, const NS_SNMP OctetStr&,
				    int&, int&, const int&, NS_SNMP OctetStr&);

  boolean		addNewRow(const NS_SNMP OctetStr& groupName, 
				  const NS_SNMP OctetStr& prefix, 
				  int securityModel, int securityLevel,
				  int match, const NS_SNMP OctetStr& readView, 
				  const NS_SNMP OctetStr& writeView,
				  const NS_SNMP OctetStr& notifyView, int storageType);
  void deleteRow(const NS_SNMP OctetStr& groupName, const NS_SNMP OctetStr& prefix, 
                 int securityModel, int securityLevel);
 protected:
  VacmSecurityToGroupTable* securityToGroupTable;

};

class AGENTPP_DECL VacmAccessTableStatus: public snmpRowStatus 
{
 public:
  VacmAccessTableStatus(const Oidx&, int);
  virtual ~VacmAccessTableStatus() { }

  virtual MibEntryPtr	clone();

 protected:
  int base_len;

};

class AGENTPP_DECL ViewNameIndex {

 public:
  ViewNameIndex(const NS_SNMP OctetStr& vname): name(vname) { }
  ~ViewNameIndex() 
    { /* avoid deletion of original rows: */ views.clear(); }

  void		add(MibTableRow* row)    { views.add(row); }
  void		remove(MibTableRow* row) { views.remove(row); }
  boolean	isEmpty() { return views.empty(); }

  NS_SNMP OctetStr		name;
  List<MibTableRow>	views;
};


/*********************************************************************

               VacmViewTreeFamilyTable

 ********************************************************************/
#if !defined (AGENTPP_DECL_TEMPL_LIST_VIEWNAMEINDEX)
#define AGENTPP_DECL_TEMPL_LIST_VIEWNAMEINDEX
	AGENTPP_DECL_TEMPL template class AGENTPP_DECL List<ViewNameIndex>;
#endif

class AGENTPP_DECL VacmViewTreeFamilyTable: public StorageTable {
friend class VacmViewTreeFamilyTableStatus;
public:
  VacmViewTreeFamilyTable();
  virtual ~VacmViewTreeFamilyTable();

  virtual boolean	ready_for_service(Vbx*, int);
  virtual boolean	could_ever_be_managed(const Oidx&, int&);
  virtual void		row_added(MibTableRow*, const Oidx&, MibTable*);
  virtual void		row_activated(MibTableRow*, const Oidx&, MibTable*);
  virtual void		row_deactivated(MibTableRow*, const Oidx&, MibTable*);
  virtual void		row_delete(MibTableRow*, const Oidx&, MibTable*);
  virtual void   	row_init(MibTableRow*, const Oidx&, MibTable* t=0);
  boolean	isInMibView(const NS_SNMP OctetStr&, const Oidx&);
  boolean	addNewRow(const NS_SNMP OctetStr& viewName, const Oidx& subtree, 
			  const NS_SNMP OctetStr& mask, int type, int storageType);
  void          deleteRow(const NS_SNMP OctetStr& viewName, const Oidx& subtree);
  virtual void		clear() { StorageTable::clear(); buildViewNameIndex();}
  virtual void		reset() { StorageTable::reset(); buildViewNameIndex();}

protected:
  boolean bit(unsigned int, NS_SNMP OctetStr&);
  ViewNameIndex* viewsOf(const NS_SNMP OctetStr& viewName);  
  void		buildViewNameIndex();

  List<ViewNameIndex> viewNameIndex;
};


class AGENTPP_DECL VacmViewTreeFamilyTableStatus: public snmpRowStatus 
{
 public:
  VacmViewTreeFamilyTableStatus(const Oidx&, int);
  virtual ~VacmViewTreeFamilyTableStatus() { }
  virtual int		set(const Vbx&);
  virtual MibEntryPtr	clone();

 protected:
  int base_len;
};


/*********************************************************************

               VacmMIB

 ********************************************************************/
class AGENTPP_DECL VacmMIB: public MibGroup {

 public:
         VacmMIB(VacmClassPointers);
};

#ifdef AGENTPP_NAMESPACE
}
#endif
#endif

