#ifndef __gdcmAttributeFactory_h
#define __gdcmAttributeFactory_h

#include "gdcmType.h"
#include "gdcmVR.h"
#include "gdcmTag.h"
#include "gdcmVM.h"
#include <string>
#include <vector>
#include <sstream>

namespace gdcm
{

typedef enum {
  ASCII_TYPE = 0,
  BINARY_TYPE
} AFModes; // AttributeFactory Modes
template<int T> struct TypeEnumToMode;
template<> struct TypeEnumToMode<VR::FL>
{ enum { Mode = BINARY_TYPE }; };
template<> struct TypeEnumToMode<VR::FD>
{ enum { Mode = BINARY_TYPE }; };
template<> struct TypeEnumToMode<VR::SL>
{ enum { Mode = BINARY_TYPE }; };
template<> struct TypeEnumToMode<VR::SS>
{ enum { Mode = BINARY_TYPE }; };
template<> struct TypeEnumToMode<VR::UL>
{ enum { Mode = BINARY_TYPE }; };
template<> struct TypeEnumToMode<VR::US>
{ enum { Mode = BINARY_TYPE }; };
template<> struct TypeEnumToMode<VR::AT>
{ enum { Mode = BINARY_TYPE }; };

template<int T> struct TypeEnumToType;
template<> struct TypeEnumToType<VR::FL>
{ typedef float Type; };
template<> struct TypeEnumToType<VR::FD>
{ typedef double Type; };
template<> struct TypeEnumToType<VR::SL>
{ typedef int32_t Type; };
template<> struct TypeEnumToType<VR::SS>
{ typedef int16_t Type; };
template<> struct TypeEnumToType<VR::UL>
{ typedef uint32_t Type; };
template<> struct TypeEnumToType<VR::US>
{ typedef uint16_t Type; };
template<> struct TypeEnumToType<VR::AT>
{ typedef Tag Type; };

template<int T> struct ValueEnumToLength;
template<> struct ValueEnumToLength<VM::VM1>
{ enum { Len = 1 }; };
template<> struct ValueEnumToLength<VM::VM2>
{ enum { Len = 2 }; };
template<> struct ValueEnumToLength<VM::VM3>
{ enum { Len = 3 }; };
template<> struct ValueEnumToLength<VM::VM4>
{ enum { Len = 4 }; };
template<> struct ValueEnumToLength<VM::VM5>
{ enum { Len = 5 }; };
template<> struct ValueEnumToLength<VM::VM6>
{ enum { Len = 6 }; };
template<> struct ValueEnumToLength<VM::VM8>
{ enum { Len = 8 }; };
template<> struct ValueEnumToLength<VM::VM16>
{ enum { Len = 16 }; };
template<> struct ValueEnumToLength<VM::VM24>
{ enum { Len = 24 }; };
template<> struct ValueEnumToLength<VM::VM1_32>
{ enum { Len = 32 }; };
template<> struct ValueEnumToLength<VM::VM1_99>
{ enum { Len = 99 }; };
//template<> struct ValueEnumToLength<VM::VM1_n>
//{ enum { Len = 3 }; };
//FIXME TODO: 2-2n and 3-3n...


// Declaration, also serve as forward declaration
template<int T> class ModeImplementation;

template<int TVR, int TVM>
class AttributeFactory
{
public:
  typename TypeEnumToType<TVR>::Type Internal[ValueEnumToLength<TVM>::Len];

//  unsigned int GetTypeSize() const {
//    return sizeof(typename TypeEnumToType<TVR>::Type);
//  }
  int GetLength() const {
    return ValueEnumToLength<TVM>::Len;
  }
  // Implementation of Print is common to all Mode (ASCII/Binary)
  void Print(std::ostream &_os) const {
    _os << Internal[0]; // VM is at least garantee to be one
    for(int i=1; i<ValueEnumToLength<TVM>::Len; ++i)
      _os << "\\" << Internal[i];
    }

  void Read(std::istream &_is) {
    ModeImplementation<TypeEnumToMode<TVR>::Mode>::Read(Internal, 
      GetLength(),_is);
    }
  void Write(std::ostream &_os) const {
    ModeImplementation<TypeEnumToMode<TVR>::Mode>::Write(Internal, 
      GetLength(),_os);
    }
};

// Implementation to perform formatted read and write
template<> class ModeImplementation<ASCII_TYPE> {
public:
  template<typename T> // FIXME this should be TypeEnumToType<TVR>::Type
  static inline void Read(const T* data, unsigned long length,
                          std::istream &_is) {
    assert( data );
    assert( length ); // != 0
    assert( _is );
    _is >> data[0];
    char sep;
    //std::cout << "GetLength: " << af->GetLength() << std::endl;
    for(unsigned long i=1; i<length;++i) {
      assert( _is );
      // Get the separator in between the values
      _is.get(sep);
      assert( sep == '\\' ); // FIXME: Bad use of assert
      _is >> data[i];
      }
    }

  template<typename T>
  static inline void Write(const T* data, unsigned long length,
                           std::ostream &_os)  {
    assert( data );
    assert( length );
    assert( _os );
    _os << data[0];
    for(int i=1; i<length; ++i) {
      assert( _os );
      _os << "\\" << data[i];
      }
    }
};

// Implementation to perform binary read and write
// TODO rewrite operation so that either:
// #1. dummy implementation use a pointer to Internal and do ++p (faster)
// #2. Actually do some meta programming to unroll the loop 
// (no notion of order in VM ...)
template<> class ModeImplementation<BINARY_TYPE> {
public:
  template<typename T>
  static inline void Read(T* data, unsigned long length,
    std::istream &_is) {
    const unsigned int type_size = sizeof(T);
    assert( data ); // Can we read from pointer ?
    assert( length );
    assert( _is ); // Is stream valid ?
    _is.read( reinterpret_cast<char*>(&(data[0])), type_size);
    for(unsigned long i=1; i<length; ++i) {
      assert( _is );
      _is.read( reinterpret_cast<char*>(&(data[i])), type_size );
    }
  }
  template<typename T>
  static inline void Write(const T* data, unsigned long length,
    std::ostream &_os) { 
    const unsigned int type_size = sizeof(T);
    assert( data ); // Can we write into pointer ?
    assert( length );
    assert( _os ); // Is stream valid ?
    _os.write( reinterpret_cast<const char*>(&(data[0])), type_size);
    for(unsigned long i=1; i<length;++i) {
      assert( _os );
      _os.write( reinterpret_cast<const char*>(&(data[i])), type_size );
    }
  }

};

// Implementation for the undefined length (dynamically allocated array) 
template<int TVR>
class AttributeFactory<TVR, VM::VM1_n>
{
public:
  // This the way to prevent default initialization
  explicit AttributeFactory() { Internal=0; Length=0; }
  ~AttributeFactory() {
    delete[] Internal;
    Internal = 0;
  }

  // Length manipulation
  int GetLength() const { return Length; }
  void SetLength(unsigned long len) {
    if( len ) {
      if( len > Length ) {
        // perform realloc
        typename TypeEnumToType<TVR>::Type *internal = 
          new typename TypeEnumToType<TVR>::Type[len];
        memcpy(internal, Internal, Length);
        delete[] Internal;
        Internal = internal;
        }
      }
    Length = len;
  }

  // If save is set to zero user should not delete the pointer
  //void SetArray(const typename TypeEnumToType<TVR>::Type *array, int len, bool save = false) 
  typedef typename TypeEnumToType<TVR>::Type ArrayType;
  void SetArray(ArrayType *array, unsigned long len,
    bool save = false) {
    if( save ) {
      SetLength(len); // realloc
      memcpy(Internal, array, len*sizeof(ArrayType));
      }
    else {
      // TODO rewrite this stupid code:
      Length = len;
      Internal = array;
      }
  }
  // Implementation of Print is common to all Mode (ASCII/Binary)
  void Print(std::ostream &_os) const {
    _os << Internal[0]; // VM is at least garantee to be one
    for(int i=1; i<GetLength(); ++i)
      _os << "\\" << Internal[i];
    }
  void Read(std::istream &_is) {
    ModeImplementation<TypeEnumToMode<TVR>::Mode>::Read(Internal, 
      GetLength(),_is);
    }
  void Write(std::ostream &_os) const {
    ModeImplementation<TypeEnumToMode<TVR>::Mode>::Write(Internal, 
      GetLength(),_os);
    }

  AttributeFactory(const AttributeFactory&_val) {
    if( this != &_val) {
      *this = _val;
      }
    }

  AttributeFactory &operator=(const AttributeFactory &_val) {
    Length = 0; // SYITF
    Internal = 0;
    SetArray(_val.Internal, _val.Length, true);
    return *this;
    }

private:
  typename TypeEnumToType<TVR>::Type *Internal;
  unsigned long Length; // unsigned int ??
};

// Partial specialization for derivatives of 1-n : 2-n, 3-n ...
template<int TVR>
class AttributeFactory<TVR, VM::VM2_n> : public AttributeFactory<TVR, VM::VM1_n>
{
public:
  typedef AttributeFactory<TVR, VM::VM1_n> Parent;
  void SetLength(int len) {
    if( len <= 1 ) return;
    Parent::SetLength(len);
  }
};
template<int TVR>
class AttributeFactory<TVR, VM::VM2_2n> : public AttributeFactory<TVR, VM::VM2_n>
{
public:
  typedef AttributeFactory<TVR, VM::VM2_n> Parent;
  void SetLength(int len) {
    if( len % 2 ) return;
    Parent::SetLength(len);
  }
};
template<int TVR>
class AttributeFactory<TVR, VM::VM3_n> : public AttributeFactory<TVR, VM::VM1_n>
{
public:
  typedef AttributeFactory<TVR, VM::VM1_n> Parent;
  void SetLength(int len) {
    if( len <= 2 ) return;
    Parent::SetLength(len);
  }
};
template<int TVR>
class AttributeFactory<TVR, VM::VM3_3n> : public AttributeFactory<TVR, VM::VM3_n>
{
public:
  typedef AttributeFactory<TVR, VM::VM3_n> Parent;
  void SetLength(int len) {
    if( len % 3 ) return;
    Parent::SetLength(len);
  }
};

} // namespace gdcm

#endif //__gdcmAttributeFactory_h
