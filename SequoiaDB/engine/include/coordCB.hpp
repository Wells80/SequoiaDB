
#ifndef COORDCB_HPP__
#define COORDCB_HPP__

#include "core.hpp"
#include "oss.hpp"
#include <map>
#include <boost/shared_ptr.hpp>
#include "netRouteAgent.hpp"
#include "netMultiRouteAgent.hpp"
#include "rtnCoord.hpp"
#include "ossUtil.h"
#include "rtnPredicate.hpp"
#include "msgCatalog.hpp"
#include "clsCatalogAgent.hpp"
#include "coordDef.hpp"
#include "sdbInterface.hpp"

namespace engine
{
   /*
      _CoordCB define
   */
   class _CoordCB : public _IControlBlock
   {
      typedef std::map<std::string, UINT32>     GROUP_NAME_MAP ;
      typedef GROUP_NAME_MAP::iterator          GROUP_NAME_MAP_IT ;

   public:
      _CoordCB() ;
      virtual ~_CoordCB() ;

      virtual SDB_CB_TYPE cbType() const { return SDB_CB_COORD ; }
      virtual const CHAR* cbName() const { return "COORDCB" ; }

      virtual INT32  init () ;
      virtual INT32  active () ;
      virtual INT32  deactive () ;
      virtual INT32  fini () ;
      virtual void   onConfigChange() ;

      _netRouteAgent* netWork() { return _pNetWork ; }
      netMultiRouteAgent* getRouteAgent() { return &_multiRouteAgent ; }

      rtnCoordProcesserFactory *getProcesserFactory()
      {
         return &_processerFactory ;
      }

      void getLock( OSS_LATCH_MODE mode )
      {
         if ( SHARED == mode )
         {
            _mutex.get_shared() ;
         }
         else
         {
            _mutex.get() ;
         }
      }
      void releaseLock( OSS_LATCH_MODE mode )
      {
         if ( SHARED == mode )
         {
            _mutex.release_shared() ;
         }
         else
         {
            _mutex.release() ;
         }
      }

      INT32 addCatNodeAddr( const _MsgRouteID &id,
                            const CHAR *pHost,
                            const CHAR *pService );

      void getCatNodeAddrList ( CoordVecNodeInfo &catNodeLst );

      void clearCatNodeAddrList();

      void updateCatGroupInfo( CoordGroupInfoPtr &groupInfo );

      CoordGroupInfoPtr getCatGroupInfo()
      {
         CoordGroupInfoPtr catGroupInfoTmp;
         {
            ossScopedLock _lock(&_mutex, SHARED) ;
            catGroupInfoTmp = _catGroupInfo ;
         }
         return catGroupInfoTmp;
      }

      MsgRouteID getPrimaryCat()
      {
         ossScopedLock _lock(&_mutex, SHARED) ;
         return _catGroupInfo->getPrimary();
      }

      void setSlaveCat( MsgRouteID slave )
      {
         ossScopedLock _lock(&_mutex, EXCLUSIVE) ;
         _catGroupInfo->setSlave( slave );
      }

      void setPrimaryCat( MsgRouteID primary )
      {
         ossScopedLock _lock(&_mutex, EXCLUSIVE) ;
         _catGroupInfo->setPrimary( primary );
      }

      INT32 groupID2Name ( UINT32 id, std::string &name ) ;

      INT32 groupName2ID ( const CHAR* name, UINT32 &id ) ;

      void  addGroupInfo ( CoordGroupInfoPtr &groupInfo ) ;

      void  removeGroupInfo( UINT32 groupID ) ;

      INT32 getGroupInfo ( UINT32 groupID, CoordGroupInfoPtr &groupInfo ) ;

      INT32 getGroupInfo ( const CHAR *groupName,
                           CoordGroupInfoPtr &groupInfo ) ;

      void updateCataInfo ( const std::string &collectionName,
                            CoordCataInfoPtr &cataInfo ) ;

      INT32 getCataInfo ( const std::string &strCollectionName,
                          CoordCataInfoPtr &cataInfo ) ;

      void delCataInfo ( const std::string &collectionName ) ;

      void invalidateCataInfo() ;

   protected:
      INT32         _addGroupName ( const std::string& name, UINT32 id ) ;
      INT32         _clearGroupName ( UINT32 id ) ;

   private:
      _netRouteAgent                      *_pNetWork;
      ossSpinSLatch                       _mutex;
      netMultiRouteAgent                  _multiRouteAgent;
      CoordGroupInfoPtr                   _catGroupInfo;//don't modify while runtime
      rtnCoordProcesserFactory            _processerFactory;

      ossSpinSLatch                       _nodeGroupMutex;
      CoordGroupMap                       _nodeGroupInfo;
      GROUP_NAME_MAP                      _groupNameMap ;

      ossSpinSLatch                       _cataInfoMutex;
      CoordCataMap                        _cataInfoMap;

      CoordVecNodeInfo                    _cataNodeAddrList;

   };
   typedef _CoordCB CoordCB ;

   /*
      get global coord cb
   */
   CoordCB* sdbGetCoordCB() ;

}

#endif // COORDCB_HPP__

