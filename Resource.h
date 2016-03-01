#pragma once
#include <unordered_map>
#include <memory>

namespace itc
{
	using namespace std;

	////////////////////////////////////////////////////////////////////////////

	/**
	 * Resource ref just keeps a track of references. Does not call resource destructors.
	 */
	template<class T> struct Resource
	{
		struct ResType
		{
			T   obj;
			int refs;
			ResType() : refs(1) {}
			template<class T, class...U> ResType(U&&...args) : refs(1), obj(forward<U>(args)...) {}
		};
		ResType* ref;
		Resource()           : ref(nullptr) {}
		Resource(ResType* r) : ref(r) {}
		~Resource() { decref(); }
		Resource(Resource&& fwd)      : ref(fwd.ref) { fwd.ref = nullptr; }
		Resource(const Resource& rhs) : ref(fwd.ref) { addref(); }
		Resource& operator=(Resource&& fwd)      { swap(ref, fwd.ref); return *this; }
		Resource& operator=(const Resource& rhs) { set(rhs.ref);       return *this; }
		void set(ResType* r) { decref(); ref=r; addref(); }
		void addref() { if (ref) ++ref->refs; }
		void decref() { if (ref) --ref->refs; }
		int numrefs() const { return ref->refs; }
		T* operator->() { return &ref->obj; }
		T* get() const  { return ref ? &ref->obj : nullptr; }
		operator bool() const { return !!ref; }
	};


	/**
	 * ResourceManager resources are not automatically destroyed
	 * and must be unloaded manually.
	 */
	template<class T> struct ResourceManager
	{
		typedef typename itc::Resource<T>          Resource;
		typedef typename itc::Resource<T>::ResType ResType;

		unordered_map<string, Resource> Resources;

		~ResourceManager()
		{
			destroyAll();
		}

		Resource& getResource(const string& resourcePath)
		{
			Resource& item = Resources[resourcePath];
			if (item) return item;
			item.set(new ResType(resourcePath));
			return item;
		}

		/** @brief Frees all unused resources */
		void freeUnused()
		{
			for (auto it = Resources.begin(); it != Resources.end(); ++it)
			{
				if (!it->second)
				{
					it = Resources.erase(it);
				}
				else if (it->second.refcount() == 0) 
				{
					delete it->second.ref;
					it = Resources.erase(it);
				}
			}
		}

		/** @brief Destroys all resources, regardless of refcounts */
		void destroyAll()
		{
			for (auto it = Resources.begin(); it != Resources.end(); ++it)
			{
				delete it->second.ref;
				it = Resources.erase(it);
			}
		}
		
	};

	////////////////////////////////////////////////////////////////////////////
}