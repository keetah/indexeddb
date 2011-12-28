/**********************************************************\
Copyright Brandon Haynes
http://code.google.com/p/indexeddb
GNU Lesser General Public License
\**********************************************************/

#include "BrowserObjectAPI.h"
#include "ObjectStoreSync.h"
#include "IndexSync.h"
#include "../../Implementation/Index.h"
#include "../../Implementation/KeyGenerator.h"
#include "../../Implementation/AbstractDatabaseFactory.h"
#include "../../Support/privateObservable.h"
#include "../../Support/KeyPathKeyGenerator.h"
#include "../../Support/Convert.h"
#include "../DatabaseException.h"

using std::auto_ptr;
using std::string;
using boost::optional;

namespace BrandonHaynes {
namespace IndexedDB { 

using Implementation::ImplementationException;
using Implementation::KeyGenerator;
using Implementation::Key;
using Implementation::Data;

namespace API { 

void IndexSync::addLifeCycleObserver( const LifeCycleObserverPtr& observer )
{
    _observable->addLifeCycleObserver(observer);
}

void IndexSync::removeLifeCycleObserver( const LifeCycleObserverPtr& observer )
{
    _observable->removeLifeCycleObserver(observer);
}

IndexSync::IndexSync(FB::BrowserHostPtr host, const ObjectStoreSyncPtr& objectStore, TransactionFactory& transactionFactory, Metadata& metadata, const string& name)
	: Index(name, objectStore->getName()), 
      openCursors(boost::make_shared<Support::Container<CursorSync> >()),
	  transactionFactory(transactionFactory),
	  metadata(metadata, Metadata::Index, name),
	  host(host)
	{ 
	loadMetadata();

	keyGenerator = auto_ptr<KeyGenerator>(keyPath.is_initialized()
		? new Support::KeyPathKeyGenerator(host, keyPath.get())
		: NULL);
	implementation = keyPath.is_initialized()
		? Implementation::AbstractDatabaseFactory::getInstance().openIndex(
			objectStore->getImplementation(), name, keyGenerator, unique, transactionFactory.getTransactionContext())
		: Implementation::AbstractDatabaseFactory::getInstance().openIndex(
			objectStore->getImplementation(), name, unique, transactionFactory.getTransactionContext());
	initializeMethods();
	}

IndexSync::IndexSync(FB::BrowserHostPtr host, const ObjectStoreSyncPtr& objectStore, TransactionFactory& transactionFactory, Metadata& metadata, const string& name, const optional<string>& keyPath, const bool unique)
	: Index(name, objectStore->getName(), keyPath, unique), 
      openCursors(boost::make_shared<Support::Container<CursorSync> >()),
	  transactionFactory(transactionFactory),
	  host(host),
	  metadata(metadata, Metadata::Index, name),
	  keyGenerator(keyPath.is_initialized()
		? new Support::KeyPathKeyGenerator(host, keyPath.get())
		: NULL),
	  implementation(keyPath.is_initialized()
		? Implementation::AbstractDatabaseFactory::getInstance().createIndex(
			objectStore->getImplementation(), name, keyGenerator, unique, transactionFactory.getTransactionContext())
		: Implementation::AbstractDatabaseFactory::getInstance().createIndex(
			objectStore->getImplementation(), name, unique, transactionFactory.getTransactionContext()))
	{ 
	createMetadata(keyPath, unique);
	initializeMethods(); 
	}

IndexSync::~IndexSync(void)
	{
    close();
    FB::ptr_cast<Support::_privateObservable<IndexSync> >(_observable)->invalidate();
    }

void IndexSync::initializeMethods()
	{
    _observable = boost::make_shared<Support::_privateObservable<IndexSync> >(this);
	registerMethod("get", make_method(this, &IndexSync::get));
	registerMethod("getObject", make_method(this, &IndexSync::getObject));
	registerMethod("put", make_method(this, &IndexSync::put));
	registerMethod("remove", make_method(this, &IndexSync::remove));
	registerMethod("openCursor", make_method(this, static_cast<CursorSyncPtr (IndexSync::*)(const boost::optional<FB::VariantMap>&, const boost::optional<int>&)>(&IndexSync::openCursor))); 
	registerMethod("openObjectCursor", make_method(this, &IndexSync::openObjectCursor)); 
	}

FB::variant IndexSync::get(FB::variant key)
	{ 
	try
		{ 
		Key& primaryKey = implementation->getPrimaryKey(Convert::toKey(host, key), transactionFactory.getTransactionContext()); 

		if(primaryKey.getType() == Data::Undefined)
			throw DatabaseException("NOT_FOUND_ERR", DatabaseException::NOT_FOUND_ERR);
		else
			return Convert::toVariant(host, primaryKey); 
		}
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

FB::variant IndexSync::getObject(FB::variant key)
	{
	try
		{
		Data& data = implementation->get(Convert::toKey(host, key), transactionFactory.getTransactionContext());

		if(data.getType() == Data::Undefined)
			throw DatabaseException(DatabaseException::NOT_FOUND_ERR);
		else
			return Convert::toVariant(host, data); 
		}
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

FB::variant IndexSync::put(FB::variant value, const FB::CatchAll& args)
	{
	const FB::VariantList& values = args.value;

	if(keyPath.is_initialized())
		throw DatabaseException("CONSTRAINT_ERR", DatabaseException::CONSTRAINT_ERR);
	else if(values.size() < 1)
		throw FB::invalid_arguments();
	else if(values.size() > 2)
		throw FB::invalid_arguments();
	else if(values.size() == 2 && !values[1].is_of_type<bool>())
		throw FB::invalid_arguments();
 
	FB::variant key = values[0];
	bool noOverwrite = values.size() == 2 ? values[1].cast<bool>() : false;

	try
		{ implementation->put(Convert::toKey(host, key), Convert::toData(host, value), noOverwrite, transactionFactory.getTransactionContext()); }
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }

	return key;
	}

void IndexSync::remove(FB::variant key)
	{
	try
		{ implementation->remove(Convert::toKey(host, key), transactionFactory.getTransactionContext()); }
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

void IndexSync::close()
	{ 
	_observable->raiseOnCloseEvent();
	openCursors->release();
	this->implementation->close(); 
	}

CursorSyncPtr IndexSync::openCursor(const boost::optional<FB::VariantMap>& info, const boost::optional<int>& dir)
	{
    KeyRangePtr range;
    if (info) {
        if (info->find("right") == info->end() |
            info->find("left")  == info->end() |
            info->find("flags") == info->end()) {
            throw FB::invalid_arguments();
        }
        int flags = info->find("flags")->second.convert_cast<int>();
		  //int flags =info->find("flags")->second.convert_cast<int>();

        range = boost::make_shared<KeyRange>(info->find("left")->second, info->find("right")->second, flags);
    }

    const Cursor::Direction direction = dir ? static_cast<Cursor::Direction>(*dir) : Cursor::NEXT;

	return openCursor(range, direction, true);
	}

CursorSyncPtr IndexSync::openObjectCursor(const boost::optional<FB::VariantMap>& info, const boost::optional<int>& dir)
	{
    KeyRangePtr range;
    if (info) {
        if (info->find("right") == info->end() |
            info->find("left")  == info->end() |
            info->find("flags") == info->end()) {
            throw FB::invalid_arguments();
        }
        int flags = info->find("flags")->second.convert_cast<int>();

        range = boost::make_shared<KeyRange>(info->find("left")->second, info->find("right")->second, flags);
    }

    const Cursor::Direction direction = dir ? static_cast<Cursor::Direction>(*dir) : Cursor::NEXT;

	return openCursor(range, direction, false);
	}

boost::shared_ptr<CursorSync> IndexSync::openCursor(const KeyRangePtr& range, const Cursor::Direction direction, const bool dataArePrimaryKeys)
	{ 
	try
		{ 
		CursorSyncPtr cursor(
            new CursorSync(host, FB::ptr_cast<IndexSync>(shared_from_this()), transactionFactory, range, direction, dataArePrimaryKeys)
            );
		openCursors->add(cursor);
		return cursor;
		}
	catch(ImplementationException& e)
		{ throw DatabaseException(e); }
	}

void IndexSync::createMetadata(const optional<string>& keyPath, const bool unique)
	{
	this->keyPath = keyPath;
	this->unique = unique;

	auto_ptr<Implementation::Transaction> transaction = transactionFactory.createTransaction();

	metadata.putMetadata("unique", Data(&unique, sizeof(bool), Data::Boolean), true, *transaction);
	metadata.putMetadata("keyPath", keyPath.is_initialized() ? Data(keyPath.get()) : Data::getUndefinedData(), true, *transaction);
	
	transaction->commit();
	}

void IndexSync::loadMetadata()
	{
	auto_ptr<Implementation::Transaction> transaction = transactionFactory.createTransaction();

	Data keyPathValue(metadata.getMetadata("keyPath", *transaction));
	this->keyPath = keyPathValue.getType() == Data::Undefined 
		? optional<string>()
		: optional<string>((char*)keyPathValue.getRawValue());
	this->unique = *(bool*)metadata.getMetadata("unique", *transaction).getRawValue();

	transaction->commit();
	}

void IndexSync::onTransactionCommitted(const TransactionPtr& transaction)
	{ 
	openCursors->raiseTransactionCommitted(transaction); 
	openCursors->release();
	}

void IndexSync::onTransactionAborted(const TransactionPtr& transaction)
	{ 
	openCursors->raiseTransactionAborted(transaction); 
	openCursors->release();
	}

}
}
}