/*
* Botan 3.12.0 Amalgamation
* (C) 1999-2023 The Botan Authors
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_AMALGAMATION_H_
#define BOTAN_AMALGAMATION_H_

#include <array>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iosfwd>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

/**
* @file  build.h
* @brief Build configuration for Botan 3.12.0
*/

/* NOLINTBEGIN(*-macro-usage,*-macro-to-enum) */

/**
 * @defgroup buildinfo Build Information
 */

/**
 * @ingroup buildinfo
 * @defgroup buildinfo_version Build version information
 * @{
 */

/**
* The major version of the release
*/
#define BOTAN_VERSION_MAJOR 3

/**
* The minor version of the release
*/
#define BOTAN_VERSION_MINOR 12

/**
* The patch version of the release
*/
#define BOTAN_VERSION_PATCH 0

/**
 * Expands to an integer of the form YYYYMMDD if this is an official
 * release, or 0 otherwise. For instance, 2.19.0, which was released
 * on January 19, 2022, has a `BOTAN_VERSION_DATESTAMP` of 20220119.
 *
 * This macro is deprecated; use version_datestamp from version.h
 *
 * TODO(Botan4) remove this
 */
#define BOTAN_VERSION_DATESTAMP 20260506

/**
 * A string set to the release type
 *
 * This macro is deprecated
 *
 * TODO(Botan4) remove this
 */
#define BOTAN_VERSION_RELEASE_TYPE "release"

/**
 * A macro expanding to a string that is set to a revision identifier
 * corresponding to the source, or "unknown" if this could not be
 * determined. It is set for all official releases.
 *
 * This macro is deprecated; use version_vc_revision from version.h
 *
 * TODO(Botan4) remove this
 */
#define BOTAN_VERSION_VC_REVISION "git:45d6f286c320b2f2efd5373d195ec88c367e3071"

/**
 * A macro expanding to a string that is set at build time using the
 * `--distribution-info` option. It allows a packager of the library
 * to specify any distribution-specific patches. If no value is given
 * at build time, the value is the string "unspecified".
 *
 * This macro is deprecated; use version_distribution_info from version.h
 *
 * TODO(Botan4) remove this
 */
#define BOTAN_DISTRIBUTION_INFO "unspecified"

/**
 * @}
 */

/**
 * @ingroup buildinfo
 * @defgroup buildinfo_configuration Build configurations
 * @{
 */




#ifndef BOTAN_DLL
  #define BOTAN_DLL 
#endif

/* Target identification and feature test macros */

#define BOTAN_TARGET_OS_HAS_FILESYSTEM
#define BOTAN_TARGET_OS_HAS_THREADS



/**
 * @}
 */

/**
 * @ingroup buildinfo
 * @defgroup buildinfo_modules Enabled modules and API versions
 * @{
 */

/*
* Module availability definitions
*/
#define BOTAN_HAS_AES 20131128
#define BOTAN_HAS_ASN1 20201106
#define BOTAN_HAS_AUTO_RNG 20161126
#define BOTAN_HAS_AUTO_SEEDING_RNG 20160821
#define BOTAN_HAS_BASE64_CODEC 20131128
#define BOTAN_HAS_BIGINT 20240529
#define BOTAN_HAS_BLOCK_CIPHER 20131128
#define BOTAN_HAS_CERTSTOR_SQL 20160818
#define BOTAN_HAS_CERTSTOR_SQLITE3 20160818
#define BOTAN_HAS_CIPHER_MODES 20180124
#define BOTAN_HAS_CIPHER_MODE_PADDING 20131128
#define BOTAN_HAS_ECC_GROUP 20250101
#define BOTAN_HAS_ECC_KEY 20190801
#define BOTAN_HAS_ECC_PUBLIC_KEY_CRYPTO 20131128
#define BOTAN_HAS_ECDSA 20131128
#define BOTAN_HAS_EC_HASH_TO_CURVE 20210420
#define BOTAN_HAS_HASH 20180112
#define BOTAN_HAS_HEX_CODEC 20131128
#define BOTAN_HAS_HMAC 20131128
#define BOTAN_HAS_HMAC_DRBG 20140319
#define BOTAN_HAS_KDF 20250528
#define BOTAN_HAS_KDF_BASE 20131128
#define BOTAN_HAS_MAC 20150626
#define BOTAN_HAS_MODES 20150626
#define BOTAN_HAS_MODE_CBC 20131128
#define BOTAN_HAS_NUMBERTHEORY 20201108
#define BOTAN_HAS_OCSP 20201106
#define BOTAN_HAS_PASSWORD_HASHING 20210419
#define BOTAN_HAS_PBKDF 20180902
#define BOTAN_HAS_PBKDF2 20180902
#define BOTAN_HAS_PEM_CODEC 20131128
#define BOTAN_HAS_PKCS11 20160219
#define BOTAN_HAS_PKCS5_PBES2 20141119
#define BOTAN_HAS_PUBLIC_KEY_CRYPTO 20131128
#define BOTAN_HAS_RSA_ENCRYPTION_PADDING 20250720
#define BOTAN_HAS_SHA1 20131128
#define BOTAN_HAS_SHA2_32 20131128
#define BOTAN_HAS_SHA2_64 20131128
#define BOTAN_HAS_SHA_224 20250130
#define BOTAN_HAS_SHA_256 20250130
#define BOTAN_HAS_SHA_384 20250130
#define BOTAN_HAS_SHA_512 20250130
#define BOTAN_HAS_SHA_512_256 20250130
#define BOTAN_HAS_STATEFUL_RNG 20160819
#define BOTAN_HAS_SYSTEM_RNG 20141202
#define BOTAN_HAS_X509 20201106
#define BOTAN_HAS_X509_CERTIFICATES 20201106


/*
* Internal module feature definitions
*
* These macros have been in the past visible in build.h as feature macros
* but in the future these will be only visible in an internal header.
* Applications should not rely on or check for these macros.
*/
#define BOTAN_HAS_BIGINT_MP 20151225
#define BOTAN_HAS_DYNAMIC_LOADER 20160310
#define BOTAN_HAS_MDX_HASH_FUNCTION 20131128
#define BOTAN_HAS_OS_UTILS 20241202
#define BOTAN_HAS_PCURVES 20240404
#define BOTAN_HAS_PCURVES_IMPL 20240714
#define BOTAN_HAS_PCURVES_SECP256R1 20240608
#define BOTAN_HAS_PCURVES_SECP384R1 20240608
#define BOTAN_HAS_PCURVES_SECP521R1 20240608
#define BOTAN_HAS_SQLITE3 20171118


/**
 * @}
 */

/* NOLINTEND(*-macro-usage,*-macro-to-enum) */

// NOLINTBEGIN(*-macro-usage)

/**
* Used to annotate API exports which are public and supported.
* These APIs will not be broken/removed unless strictly required for
* functionality or security, and only in new major versions.
* @param maj The major version this public API was released in
* @param min The minor version this public API was released in
*/
#define BOTAN_PUBLIC_API(maj, min) BOTAN_DLL

/**
* Used to annotate API exports which are public, but are now deprecated
* and which will be removed in a future major release.
*/
#define BOTAN_DEPRECATED_API(msg) BOTAN_DEPRECATED(msg) BOTAN_DLL

/**
* Used to annotate API exports which are public and can be used by
* applications if needed, but which are intentionally not documented,
* and which may change incompatibly in a future major version.
*/
#define BOTAN_UNSTABLE_API BOTAN_DLL

/**
* Used to annotate API exports which are exported but only for the
* purposes of testing. They should not be used by applications and
* may be removed or changed without notice.
*/
#define BOTAN_TEST_API BOTAN_DLL

/**
* This is used to mark constructors which are currently not `explicit`
* but which in a future major release be modified as such.
*
* TODO(Botan4) remove this macro and replace with `explicit`
*/
#if defined(__clang_analyzer__) || defined(BOTAN_DISABLE_DEPRECATED_FEATURES)
   #define BOTAN_FUTURE_EXPLICIT explicit
#else
   #define BOTAN_FUTURE_EXPLICIT
#endif

/**
* Used to annotate API exports which are exported but only for the
* purposes of fuzzing. They should not be used by applications and
* may be removed or changed without notice.
*
* They are only exported if the fuzzers are being built
*/
#if defined(BOTAN_FUZZERS_ARE_BEING_BUILT)
   #define BOTAN_FUZZER_API BOTAN_DLL
#else
   #define BOTAN_FUZZER_API
#endif

/*
* Define BOTAN_DEPRECATED
*/
#if !defined(BOTAN_NO_DEPRECATED_WARNINGS) && !defined(BOTAN_AMALGAMATION_H_) && !defined(BOTAN_IS_BEING_BUILT)

   #define BOTAN_DEPRECATED(msg) [[deprecated(msg)]]

   #if defined(__clang__)
      #define BOTAN_DEPRECATED_HEADER(hdr) _Pragma("message \"this header is deprecated\"")
      #define BOTAN_FUTURE_INTERNAL_HEADER(hdr) _Pragma("message \"this header will be made internal in the future\"")
   #elif defined(_MSC_VER)
      #define BOTAN_DEPRECATED_HEADER(hdr) __pragma(message("this header is deprecated"))
      #define BOTAN_FUTURE_INTERNAL_HEADER(hdr) __pragma(message("this header will be made internal in the future"))
   #elif defined(__GNUC__)
      #define BOTAN_DEPRECATED_HEADER(hdr) _Pragma("GCC warning \"this header is deprecated\"")
      #define BOTAN_FUTURE_INTERNAL_HEADER(hdr) \
         _Pragma("GCC warning \"this header will be made internal in the future\"")
   #endif

#endif

#if !defined(BOTAN_DEPRECATED)
   #define BOTAN_DEPRECATED(msg)
#endif

#if !defined(BOTAN_DEPRECATED_HEADER)
   #define BOTAN_DEPRECATED_HEADER(hdr)
#endif

#if !defined(BOTAN_FUTURE_INTERNAL_HEADER)
   #define BOTAN_FUTURE_INTERNAL_HEADER(hdr)
#endif

#if defined(__clang__)
   #define BOTAN_DIAGNOSTIC_PUSH _Pragma("clang diagnostic push")
   #define BOTAN_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS \
      _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")
   #define BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE
   #define BOTAN_DIAGNOSTIC_POP _Pragma("clang diagnostic pop")
#elif defined(__GNUG__)
   #define BOTAN_DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
   #define BOTAN_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS \
      _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
   #define BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE
   #define BOTAN_DIAGNOSTIC_POP _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
   #define BOTAN_DIAGNOSTIC_PUSH __pragma(warning(push))
   #define BOTAN_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS __pragma(warning(disable : 4996))
   #define BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE __pragma(warning(disable : 4250))
   #define BOTAN_DIAGNOSTIC_POP __pragma(warning(pop))
#else
   #define BOTAN_DIAGNOSTIC_PUSH
   #define BOTAN_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS
   #define BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE
   #define BOTAN_DIAGNOSTIC_POP
#endif

// NOLINTEND(*-macro-usage)

/**
* MSVC does define __cplusplus but pins it at 199711L, because "legacy".
* Note: There is a compiler switch to enable standard behavior (/Zc:__cplusplus),
*       but we can't control that in downstream applications.
*
* See: https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus
*/
#if defined(_MSVC_LANG)
   #define BOTAN_CPLUSPLUS _MSVC_LANG
#else
   #define BOTAN_CPLUSPLUS __cplusplus
#endif

#if BOTAN_CPLUSPLUS < 202002L
   #error "Botan 3.x requires at least C++20"
#endif

namespace Botan {

/**
* @mainpage Botan Crypto Library API Reference
*
* <dl>
* <dt>Abstract Base Classes<dd>
*        BlockCipher, HashFunction, KDF, MessageAuthenticationCode, RandomNumberGenerator,
*        StreamCipher, SymmetricAlgorithm, AEAD_Mode, Cipher_Mode, XOF
* <dt>Public Key Interface Classes<dd>
*        PK_Key_Agreement, PK_Signer, PK_Verifier, PK_Encryptor, PK_Decryptor, PK_KEM_Encryptor, PK_KEM_Decryptor
* <dt>Authenticated Encryption Modes<dd>
*        @ref Ascon_AEAD128 "Ascon-AEAD128" @ref CCM_Mode "CCM", @ref ChaCha20Poly1305_Mode "ChaCha20Poly1305",
*        @ref EAX_Mode "EAX", @ref GCM_Mode "GCM", @ref OCB_Mode "OCB", @ref SIV_Mode "SIV"
* <dt>Block Ciphers<dd>
*        @ref aria.h "ARIA", @ref aes.h "AES", @ref Blowfish, @ref camellia.h "Camellia", @ref Cascade_Cipher "Cascade",
*        @ref CAST_128 "CAST-128", @ref CAST_128 DES, @ref TripleDES "3DES",
*        @ref GOST_28147_89 "GOST 28147-89", IDEA, Kuznyechik, Lion, Noekeon, SEED, Serpent, SHACAL2, SM4,
*        @ref Threefish_512 "Threefish", Twofish
* <dt>Stream Ciphers<dd>
*        ChaCha, @ref CTR_BE "CTR", OFB, RC4, Salsa20
* <dt>Hash Functions<dd>
*        @ref Ascon_Hash256 "Ascon-Hash256", BLAKE2b, @ref GOST_34_11 "GOST 34.11", @ref Keccak_1600 "Keccak", MD4,
*        MD5, @ref RIPEMD_160 "RIPEMD-160", @ref SHA_1 "SHA-1", @ref SHA_224 "SHA-224", @ref SHA_256 "SHA-256",
*        @ref SHA_384 "SHA-384", @ref SHA_512 "SHA-512", @ref Skein_512 "Skein-512", SM3, Streebog, Whirlpool
* <dt>Non-Cryptographic Checksums<dd>
*        Adler32, CRC24, CRC32
* <dt>Message Authentication Codes<dd>
*        @ref BLAKE2bMAC "BLAKE2b", CMAC, HMAC, KMAC, Poly1305, SipHash, ANSI_X919_MAC
* <dt>Random Number Generators<dd>
*        AutoSeeded_RNG, HMAC_DRBG, Processor_RNG, System_RNG
* <dt>Key Derivation<dd>
*        HKDF, @ref KDF1 "KDF1 (IEEE 1363)", @ref KDF1_18033 "KDF1 (ISO 18033-2)", @ref KDF2 "KDF2 (IEEE 1363)",
*        @ref sp800_108.h "SP800-108", @ref SP800_56C "SP800-56C", @ref PKCS5_PBKDF2 "PBKDF2 (PKCS#5)"
* <dt>Password Hashing<dd>
*        @ref argon2.h "Argon2", @ref scrypt.h "scrypt", @ref bcrypt.h "bcrypt", @ref passhash9.h "passhash9"
* <dt>Public Key Cryptosystems<dd>
*        @ref dlies.h "DLIES", @ref ecies.h "ECIES", @ref elgamal.h "ElGamal",
*        @ref rsa.h "RSA", @ref mceliece.h "McEliece", @ref sm2.h "SM2"
* <dt>Key Encapsulation Mechanisms<dd>
*        @ref cmce.h "Classic McEliece", @ref frodokem.h "FrodoKEM", @ref kyber.h "ML-KEM/Kyber", @ref rsa.h "RSA"
* <dt>Public Key Signature Schemes<dd>
*        @ref dsa.h "DSA", @ref dilithium.h "ML-DSA/Dilithium", @ref ecdsa.h "ECDSA", @ref ecgdsa.h "ECGDSA",
*        @ref eckcdsa.h "ECKCDSA", @ref gost_3410.h "GOST 34.10-2001", @ref hss_lms.h "HSS/LMS", @ref sm2.h "SM2",
         @ref sphincsplus.h "SLH-DSA/SPHINCS+", @ref xmss.h "XMSS"
* <dt>Key Agreement<dd>
*        @ref dh.h "DH", @ref ecdh.h "ECDH"
* <dt>Compression<dd>
*        @ref bzip2.h "bzip2", @ref lzma.h "lzma", @ref zlib.h "zlib"
* <dt>TLS<dd>
*        TLS::Client, TLS::Server, TLS::Policy, TLS::Protocol_Version, TLS::Callbacks, TLS::Ciphersuite,
*        TLS::Session, TLS::Session_Summary, TLS::Session_Manager, Credentials_Manager
* <dt>X.509<dd>
*        X509_Certificate, X509_CRL, X509_CA, Certificate_Extension, PKCS10_Request, X509_Cert_Options,
*        Certificate_Store, Certificate_Store_In_SQL, Certificate_Store_In_SQLite
* <dt>eXtendable Output Functions<dd>
*        @ref Ascon_XOF128 "Ascon-XOF128", @ref SHAKE_XOF "SHAKE"
* </dl>
*/

using std::int32_t;
using std::int64_t;
using std::size_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint8_t;

#if !defined(BOTAN_IS_BEING_BUILT)
/*
* These typedefs are no longer used within the library headers
* or code. They are kept only for compatibility with software
* written against older versions.
*/
using byte = std::uint8_t;
using u16bit = std::uint16_t;
using u32bit = std::uint32_t;
using u64bit = std::uint64_t;
using s32bit = std::int32_t;
#endif

static constexpr bool HasNative64BitRegisters = sizeof(void*) >= 8;

using word = std::conditional_t<HasNative64BitRegisters, std::uint64_t, uint32_t>;

#if defined(__SIZEOF_INT128__)
   #define BOTAN_TARGET_HAS_NATIVE_UINT128

// GCC complains if this isn't marked with __extension__
__extension__ typedef unsigned __int128 uint128_t;
#endif

/*
* Should this assert fail on your system please contact the developers
* for assistance in porting.
*/
static_assert(sizeof(std::size_t) == 8 || sizeof(std::size_t) == 4, "This platform has an unexpected size for size_t");

/**
* How much to allocate for a buffer of no particular size
*/
constexpr size_t DefaultBufferSize = 4096;

}  // namespace Botan

namespace Botan {

/*
* Define BOTAN_MALLOC_FN
*/
#if defined(__clang__) || defined(__GNUG__)
   #define BOTAN_MALLOC_FN __attribute__((malloc))
#elif defined(_MSC_VER)
   #define BOTAN_MALLOC_FN __declspec(restrict)
#else
   #define BOTAN_MALLOC_FN
#endif

/**
* Allocate a memory buffer by some method. This should only be used for
* primitive types (uint8_t, uint32_t, etc).
*
* @param elems the number of elements
* @param elem_size the size of each element
* @return pointer to allocated and zeroed memory, or throw std::bad_alloc on failure
*/
BOTAN_PUBLIC_API(2, 3) BOTAN_MALLOC_FN void* allocate_memory(size_t elems, size_t elem_size);

/**
* Free a pointer returned by allocate_memory
* @param p the pointer returned by allocate_memory
* @param elems the number of elements, as passed to allocate_memory
* @param elem_size the size of each element, as passed to allocate_memory
*/
BOTAN_PUBLIC_API(2, 3) void deallocate_memory(void* p, size_t elems, size_t elem_size);

/**
* Ensure the allocator is initialized
*/
void BOTAN_UNSTABLE_API initialize_allocator();

class Allocator_Initializer final {
   public:
      Allocator_Initializer() { initialize_allocator(); }
};

}  // namespace Botan

namespace Botan {

/**
* Different types of errors that might occur
*/
enum class ErrorType : uint16_t {
   /** Some unknown error */
   Unknown = 1,
   /** An error while calling a system interface */
   SystemError = 2,
   /** An operation seems valid, but not supported by the current version */
   NotImplemented = 3,
   /** Memory allocation failure */
   OutOfMemory = 4,
   /** An internal error occurred */
   InternalError = 5,
   /** An I/O error occurred */
   IoError = 6,

   /** Invalid object state */
   InvalidObjectState = 100,
   /** A key was not set on an object when this is required */
   KeyNotSet = 101,
   /** The application provided an argument which is invalid */
   InvalidArgument = 102,
   /** A key with invalid length was provided */
   InvalidKeyLength = 103,
   /** A nonce with invalid length was provided */
   InvalidNonceLength = 104,
   /** An object type was requested but cannot be found */
   LookupError = 105,
   /** Encoding a message or datum failed */
   EncodingFailure = 106,
   /** Decoding a message or datum failed */
   DecodingFailure = 107,
   /** A TLS error (error_code will be the alert type) */
   TLSError = 108,
   /** An error during an HTTP operation */
   HttpError = 109,
   /** A message with an invalid authentication tag was detected */
   InvalidTag = 110,
   /** An error during Roughtime validation */
   RoughtimeError = 111,

   /** An error when interacting with CommonCrypto API */
   CommonCryptoError = 201,
   /** An error when interacting with a PKCS11 device */
   Pkcs11Error = 202,
   /** An error when interacting with a TPM device */
   TPMError = 203,
   /** An error when interacting with a database */
   DatabaseError = 204,

   /** An error when interacting with zlib */
   ZlibError = 300,
   /** An error when interacting with bzip2 */
   Bzip2Error = 301,
   /** An error when interacting with lzma */
   LzmaError = 302,

};

//! \brief Convert an ErrorType to string
std::string BOTAN_PUBLIC_API(2, 11) to_string(ErrorType type);

/**
* Base class for all exceptions thrown by the library
*/
class BOTAN_PUBLIC_API(2, 0) Exception : public std::exception {
   public:
      /**
      * Return a descriptive string which is hopefully comprehensible to
      * a developer. It will likely not be useful for an end user.
      *
      * The string has no particular format, and the content of exception
      * messages may change from release to release. Thus the main use of this
      * function is for logging or debugging.
      */
      const char* what() const noexcept override { return m_msg.c_str(); }

      /**
      * Return the "type" of error which occurred.
      */
      virtual ErrorType error_type() const noexcept { return ErrorType::Unknown; }

      /**
      * Return an error code associated with this exception, or otherwise 0.
      *
      * The domain of this error varies depending on the source, for example on
      * POSIX systems it might be errno, while on a Windows system it might be
      * the result of GetLastError or WSAGetLastError.
      */
      virtual int error_code() const noexcept { return 0; }

      /**
      * Avoid throwing base Exception, use a subclass
      */
      explicit Exception(std::string_view msg);

      /**
      * Avoid throwing base Exception, use a subclass
      */
      Exception(const char* prefix, std::string_view msg);

      /**
      * Avoid throwing base Exception, use a subclass
      */
      Exception(std::string_view msg, const std::exception& e);

   private:
      std::string m_msg;
};

/**
* An invalid argument was provided to an API call.
*/
class BOTAN_PUBLIC_API(2, 0) Invalid_Argument : public Exception {
   public:
      explicit Invalid_Argument(std::string_view msg);

      explicit Invalid_Argument(std::string_view msg, std::string_view where);

      Invalid_Argument(std::string_view msg, const std::exception& e);

      ErrorType error_type() const noexcept override { return ErrorType::InvalidArgument; }
};

/**
* An invalid/unknown field name was passed to Public_Key::get_int_field
*/
class BOTAN_PUBLIC_API(3, 0) Unknown_PK_Field_Name final : public Invalid_Argument {
   public:
      Unknown_PK_Field_Name(std::string_view algo_name, std::string_view field_name);
};

/**
* An invalid key length was used
*/
class BOTAN_PUBLIC_API(2, 0) Invalid_Key_Length final : public Invalid_Argument {
   public:
      Invalid_Key_Length(std::string_view name, size_t length);

      ErrorType error_type() const noexcept override { return ErrorType::InvalidKeyLength; }
};

/**
* An invalid nonce length was used
*/
class BOTAN_PUBLIC_API(2, 0) Invalid_IV_Length final : public Invalid_Argument {
   public:
      Invalid_IV_Length(std::string_view mode, size_t bad_len);

      ErrorType error_type() const noexcept override { return ErrorType::InvalidNonceLength; }
};

/**
* Invalid_Algorithm_Name Exception
*/
class BOTAN_PUBLIC_API(2, 0) Invalid_Algorithm_Name final : public Invalid_Argument {
   public:
      explicit Invalid_Algorithm_Name(std::string_view name);
};

/**
* Encoding_Error Exception
*/
class BOTAN_PUBLIC_API(2, 0) Encoding_Error final : public Exception {
   public:
      explicit Encoding_Error(std::string_view name);

      ErrorType error_type() const noexcept override { return ErrorType::EncodingFailure; }
};

/**
* A decoding error occurred.
*/
class BOTAN_PUBLIC_API(2, 0) Decoding_Error : public Exception {
   public:
      explicit Decoding_Error(std::string_view name);

      Decoding_Error(std::string_view category, std::string_view err);

      Decoding_Error(std::string_view msg, const std::exception& e);

      ErrorType error_type() const noexcept override { return ErrorType::DecodingFailure; }
};

/**
* Invalid state was encountered. A request was made on an object while the
* object was in a state where the operation cannot be performed.
*/
class BOTAN_PUBLIC_API(2, 0) Invalid_State : public Exception {
   public:
      explicit Invalid_State(std::string_view err) : Exception(err) {}

      ErrorType error_type() const noexcept override { return ErrorType::InvalidObjectState; }
};

/**
* A PRNG was called on to produce output while still unseeded
*/
class BOTAN_PUBLIC_API(2, 0) PRNG_Unseeded final : public Invalid_State {
   public:
      explicit PRNG_Unseeded(std::string_view algo);
};

/**
* The key was not set on an object. This occurs with symmetric objects where
* an operation which requires the key is called prior to set_key being called.
*/
class BOTAN_PUBLIC_API(2, 4) Key_Not_Set : public Invalid_State {
   public:
      explicit Key_Not_Set(std::string_view algo);

      ErrorType error_type() const noexcept override { return ErrorType::KeyNotSet; }
};

/**
* A request was made for some kind of object which could not be located
*/
class BOTAN_PUBLIC_API(2, 0) Lookup_Error : public Exception {
   public:
      explicit Lookup_Error(std::string_view err) : Exception(err) {}

      Lookup_Error(std::string_view type, std::string_view algo, std::string_view provider = "");

      ErrorType error_type() const noexcept override { return ErrorType::LookupError; }
};

/**
* Algorithm_Not_Found Exception
*
* @warning This exception type will be removed in the future. Instead
* just catch Lookup_Error.
*/
class BOTAN_PUBLIC_API(2, 0) Algorithm_Not_Found final : public Lookup_Error {
   public:
      explicit Algorithm_Not_Found(std::string_view name);
};

/**
* Provider_Not_Found is thrown when a specific provider was requested
* but that provider is not available.
*
* @warning This exception type will be removed in the future. Instead
* just catch Lookup_Error.
*/
class BOTAN_PUBLIC_API(2, 0) Provider_Not_Found final : public Lookup_Error {
   public:
      Provider_Not_Found(std::string_view algo, std::string_view provider);
};

/**
* An AEAD or MAC check detected a message modification
*
* In versions before 2.10, Invalid_Authentication_Tag was named
* Integrity_Failure, it was renamed to make its usage more clear.
*/
class BOTAN_PUBLIC_API(2, 0) Invalid_Authentication_Tag final : public Exception {
   public:
      explicit Invalid_Authentication_Tag(std::string_view msg);

      ErrorType error_type() const noexcept override { return ErrorType::InvalidTag; }
};

/**
* For compatibility with older versions
*/
typedef Invalid_Authentication_Tag Integrity_Failure;

/**
* An error occurred while operating on an IO stream
*/
class BOTAN_PUBLIC_API(2, 0) Stream_IO_Error final : public Exception {
   public:
      explicit Stream_IO_Error(std::string_view err);

      ErrorType error_type() const noexcept override { return ErrorType::IoError; }
};

/**
* System_Error
*
* This exception is thrown in the event of an error related to interacting
* with the operating system.
*
* This exception type also (optionally) captures an integer error code eg
* POSIX errno or Windows GetLastError.
*/
class BOTAN_PUBLIC_API(2, 9) System_Error : public Exception {
   public:
      explicit System_Error(std::string_view msg) : Exception(msg), m_error_code(0) {}

      System_Error(std::string_view msg, int err_code);

      ErrorType error_type() const noexcept override { return ErrorType::SystemError; }

      int error_code() const noexcept override { return m_error_code; }

   private:
      int m_error_code;
};

/**
* An internal error occurred. If observed, please file a bug.
*/
class BOTAN_PUBLIC_API(2, 0) Internal_Error : public Exception {
   public:
      explicit Internal_Error(std::string_view err);

      ErrorType error_type() const noexcept override { return ErrorType::InternalError; }
};

/**
* Not Implemented Exception
*
* This is thrown in the situation where a requested operation is
* logically valid but is not implemented by this version of the library.
*/
class BOTAN_PUBLIC_API(2, 0) Not_Implemented final : public Exception {
   public:
      explicit Not_Implemented(std::string_view err);

      ErrorType error_type() const noexcept override { return ErrorType::NotImplemented; }
};

template <typename E, typename... Args>
inline void do_throw_error(const char* file, int line, const char* func, Args... args) {
   throw E(file, line, func, args...);
}

}  // namespace Botan

namespace Botan {

class BER_Decoder;
class DER_Encoder;
class ASN1_Time;  // in asn1_time.h
typedef ASN1_Time X509_Time;

/**
* ASN.1 Class Tags
*/
enum class ASN1_Class : uint32_t /* NOLINT(performance-enum-size) */ {
   Universal = 0b0000'0000,
   Application = 0b0100'0000,
   ContextSpecific = 0b1000'0000,
   Private = 0b1100'0000,

   Constructed = 0b0010'0000,
   ExplicitContextSpecific = Constructed | ContextSpecific,

   NoObject = 0xFF00
};

/**
* ASN.1 Type Tags
*/
enum class ASN1_Type : uint32_t /* NOLINT(performance-enum-size) */ {
   Eoc = 0x00,
   Boolean = 0x01,
   Integer = 0x02,
   BitString = 0x03,
   OctetString = 0x04,
   Null = 0x05,
   ObjectId = 0x06,
   Enumerated = 0x0A,
   Sequence = 0x10,
   Set = 0x11,

   Utf8String = 0x0C,
   NumericString = 0x12,
   PrintableString = 0x13,
   TeletexString = 0x14,
   Ia5String = 0x16,
   VisibleString = 0x1A,
   UniversalString = 0x1C,
   BmpString = 0x1E,

   UtcTime = 0x17,
   GeneralizedTime = 0x18,

   NoObject = 0xFF00,
};

inline bool intersects(ASN1_Class x, ASN1_Class y) {
   return (static_cast<uint32_t>(x) & static_cast<uint32_t>(y)) != 0;
}

inline ASN1_Type operator|(ASN1_Type x, ASN1_Type y) {
   return static_cast<ASN1_Type>(static_cast<uint32_t>(x) | static_cast<uint32_t>(y));
}

inline ASN1_Class operator|(ASN1_Class x, ASN1_Class y) {
   return static_cast<ASN1_Class>(static_cast<uint32_t>(x) | static_cast<uint32_t>(y));
}

inline uint32_t operator|(ASN1_Type x, ASN1_Class y) {
   return static_cast<uint32_t>(x) | static_cast<uint32_t>(y);
}

inline uint32_t operator|(ASN1_Class x, ASN1_Type y) {
   return static_cast<uint32_t>(x) | static_cast<uint32_t>(y);
}

std::string BOTAN_UNSTABLE_API asn1_tag_to_string(ASN1_Type type);
std::string BOTAN_UNSTABLE_API asn1_class_to_string(ASN1_Class type);

/**
* Basic ASN.1 Object Interface
*/
class BOTAN_PUBLIC_API(2, 0) ASN1_Object {
   public:
      /**
      * Encode whatever this object is into to
      * @param to the DER_Encoder that will be written to
      */
      virtual void encode_into(DER_Encoder& to) const = 0;

      /**
      * Decode whatever this object is from from
      * @param from the BER_Decoder that will be read from
      */
      virtual void decode_from(BER_Decoder& from) = 0;

      /**
      * Return the encoding of this object. This is a convenience
      * method when just one object needs to be serialized. Use
      * DER_Encoder for complicated encodings.
      */
      std::vector<uint8_t> BER_encode() const;

      ASN1_Object() = default;
      ASN1_Object(const ASN1_Object&) = default;
      ASN1_Object& operator=(const ASN1_Object&) = default;
      ASN1_Object(ASN1_Object&&) = default;
      ASN1_Object& operator=(ASN1_Object&&) = default;
      virtual ~ASN1_Object() = default;
};

/**
* BER Encoded Object
*/
class BOTAN_PUBLIC_API(2, 0) BER_Object final {
   public:
      BER_Object() = default;

      BER_Object(const BER_Object& other) = default;
      BER_Object(BER_Object&& other) = default;
      BER_Object& operator=(const BER_Object& other) = default;
      BER_Object& operator=(BER_Object&& other) = default;
      ~BER_Object();

      bool is_set() const { return m_type_tag != ASN1_Type::NoObject; }

      uint32_t tagging() const { return type_tag() | class_tag(); }

      ASN1_Type type_tag() const { return m_type_tag; }

      ASN1_Class class_tag() const { return m_class_tag; }

      ASN1_Type type() const { return m_type_tag; }

      ASN1_Class get_class() const { return m_class_tag; }

      const uint8_t* bits() const { return m_value.data(); }

      size_t length() const { return m_value.size(); }

      std::span<const uint8_t> data() const { return std::span{m_value}; }

      void assert_is_a(ASN1_Type type_tag, ASN1_Class class_tag, std::string_view descr = "object") const;

      bool is_a(ASN1_Type type_tag, ASN1_Class class_tag) const;

      bool is_a(int type_tag, ASN1_Class class_tag) const;

   private:
      ASN1_Type m_type_tag = ASN1_Type::NoObject;
      ASN1_Class m_class_tag = ASN1_Class::Universal;
      std::vector<uint8_t> m_value;

      friend class BER_Decoder;

      void set_tagging(ASN1_Type type_tag, ASN1_Class class_tag);

      uint8_t* mutable_bits(size_t length) {
         m_value.resize(length);
         return m_value.data();
      }
};

/*
* ASN.1 Utility Functions
*/
class DataSource;

namespace ASN1 {

std::vector<uint8_t> put_in_sequence(const std::vector<uint8_t>& val);
std::vector<uint8_t> put_in_sequence(const uint8_t bits[], size_t len);
std::string to_string(const BER_Object& obj);

/**
* Heuristics tests; is this object possibly BER?
* @param src a data source that will be peeked at but not modified
*/
bool maybe_BER(DataSource& src);

}  // namespace ASN1

/**
* General BER Decoding Error Exception
*/
class BOTAN_PUBLIC_API(2, 0) BER_Decoding_Error : public Decoding_Error {
   public:
      explicit BER_Decoding_Error(std::string_view err);
};

/**
* Exception For Incorrect BER Taggings
*/
class BOTAN_PUBLIC_API(2, 0) BER_Bad_Tag final : public BER_Decoding_Error {
   public:
      BER_Bad_Tag(std::string_view msg, uint32_t tagging);
};

/**
* This class represents ASN.1 object identifiers.
*/
class BOTAN_PUBLIC_API(2, 0) OID final : public ASN1_Object {
   public:
      /**
      * Create an uninitialised OID object
      */
      explicit OID() = default;

      /**
      * Construct an OID from a string.
      * @param str a string in the form "a.b.c" etc., where a,b,c are integers
      *
      * Note: it is currently required that each integer fit into 32 bits
      */
      explicit OID(std::string_view str);

      /**
      * Initialize an OID from a sequence of integer values
      */
      OID(std::initializer_list<uint32_t> init);

      /**
      * Initialize an OID from a vector of integer values
      */
      explicit OID(std::vector<uint32_t>&& init);

      /**
      * Construct an OID from a string.
      * @param str a string in the form "a.b.c" etc., where a,b,c are numbers
      *        or any known OID name (for example "RSA" or "X509v3.SubjectKeyIdentifier")
      */
      static OID from_string(std::string_view str);

      /**
      * Construct an OID from a name
      * @param name any known OID name (for example "RSA" or "X509v3.SubjectKeyIdentifier")
      */
      static std::optional<OID> from_name(std::string_view name);

      /**
      * Register a new OID in the internal table
      */
      static void register_oid(const OID& oid, std::string_view name);

      void encode_into(DER_Encoder& to) const override;
      void decode_from(BER_Decoder& from) override;

      /**
      * Find out whether this OID is empty
      * @return true is no OID value is set
      */
      bool empty() const { return m_id.empty(); }

      /**
      * Find out whether this OID has a value
      * @return true is this OID has a value
      */
      bool has_value() const { return !empty(); }

      /**
      * Get this OID as a dotted-decimal string
      * @return string representing this OID
      */
      std::string to_string() const;

      /**
      * If there is a known name associated with this OID, return that.
      * Otherwise return the result of to_string
      */
      std::string to_formatted_string() const;

      /**
      * If there is a known name associated with this OID, return that.
      * Otherwise return the empty string.
      */
      std::string human_name_or_empty() const;

      /**
      * Return true if the OID in *this is registered in the internal
      * set of constants as a known OID.
      */
      bool registered_oid() const;

      /**
      * Compare two OIDs.
      * @return true if they are equal, false otherwise
      */
      bool operator==(const OID& other) const { return m_id == other.m_id; }

      /**
      * Return a hash code for this OID
      *
      * This value is only meant as a std::unordered_map hash and
      * can change value from release to release.
      */
      uint64_t hash_code() const;

      /**
      * Check if this OID matches the provided value
      */
      bool matches(std::initializer_list<uint32_t> other) const;

      /**
      * Get this OID as list (vector) of its components.
      * @return vector representing this OID
      */
      BOTAN_DEPRECATED("Do not access the integer values, use eg to_string")
      const std::vector<uint32_t>& get_components() const {
         return m_id;
      }

      BOTAN_DEPRECATED("Do not access the integer values, use eg to_string")
      const std::vector<uint32_t>& get_id() const {
         return m_id;
      }

   private:
      std::vector<uint32_t> m_id;
};

BOTAN_PUBLIC_API(3, 0) std::ostream& operator<<(std::ostream& out, const OID& oid);

/**
* Compare two OIDs.
* @param a the first OID
* @param b the second OID
* @return true if a is not equal to b
*/
inline bool operator!=(const OID& a, const OID& b) {
   return !(a == b);
}

/**
* Compare two OIDs.
* @param a the first OID
* @param b the second OID
* @return true if a is lexicographically smaller than b
*/
BOTAN_PUBLIC_API(2, 0) bool operator<(const OID& a, const OID& b);

/**
* ASN.1 string type
* This class normalizes all inputs to a UTF-8 std::string
*/
class BOTAN_PUBLIC_API(2, 0) ASN1_String final : public ASN1_Object {
   public:
      void encode_into(DER_Encoder& to) const override;
      void decode_from(BER_Decoder& from) override;

      ASN1_Type tagging() const { return m_tag; }

      const std::string& value() const { return m_utf8_str; }

      size_t size() const { return value().size(); }

      bool empty() const { return m_utf8_str.empty(); }

      /**
      * Return true iff this is a tag for a known string type we can handle.
      */
      static bool is_string_type(ASN1_Type tag);

      bool operator==(const ASN1_String& other) const { return value() == other.value(); }

      friend bool operator<(const ASN1_String& a, const ASN1_String& b) { return a.value() < b.value(); }

      explicit ASN1_String(std::string_view utf8 = "");
      ASN1_String(std::string_view utf8, ASN1_Type tag);

   private:
      std::vector<uint8_t> m_data;
      std::string m_utf8_str;
      ASN1_Type m_tag;
};

/**
* Algorithm Identifier
*/
class BOTAN_PUBLIC_API(2, 0) AlgorithmIdentifier final : public ASN1_Object {
   public:
      enum Encoding_Option : uint8_t { USE_NULL_PARAM, USE_EMPTY_PARAM }; /* NOLINT(*-use-enum-class) */

      void encode_into(DER_Encoder& to) const override;
      void decode_from(BER_Decoder& from) override;

      AlgorithmIdentifier() = default;

      AlgorithmIdentifier(const OID& oid, Encoding_Option enc);
      AlgorithmIdentifier(std::string_view oid_name, Encoding_Option enc);

      AlgorithmIdentifier(const OID& oid, const std::vector<uint8_t>& params);
      AlgorithmIdentifier(std::string_view oid_name, const std::vector<uint8_t>& params);

      const OID& oid() const { return m_oid; }

      const std::vector<uint8_t>& parameters() const { return m_parameters; }

      BOTAN_DEPRECATED("Use AlgorithmIdentifier::oid") const OID& get_oid() const { return m_oid; }

      BOTAN_DEPRECATED("Use AlgorithmIdentifier::parameters") const std::vector<uint8_t>& get_parameters() const {
         return m_parameters;
      }

      bool parameters_are_null() const;

      bool parameters_are_empty() const { return m_parameters.empty(); }

      bool parameters_are_null_or_empty() const { return parameters_are_empty() || parameters_are_null(); }

      bool empty() const { return m_oid.empty() && m_parameters.empty(); }

   private:
      OID m_oid;
      std::vector<uint8_t> m_parameters;
};

/*
* Comparison Operations
*/
BOTAN_PUBLIC_API(2, 0) bool operator==(const AlgorithmIdentifier& x, const AlgorithmIdentifier& y);
BOTAN_PUBLIC_API(2, 0) bool operator!=(const AlgorithmIdentifier& x, const AlgorithmIdentifier& y);

}  // namespace Botan

template <>
class std::hash<Botan::OID> {
   public:
      size_t operator()(const Botan::OID& oid) const noexcept { return static_cast<size_t>(oid.hash_code()); }
};

/*
In 3.11 ASN1_Time was split out to its own header as <chrono> is huge in C++20
However we continue to include this header (when not building the library),
to avoid breaking applications which would expect it to still be available.

TODO(Botan4) remove this
*/
#if defined(BOTAN_AMALGAMATION_H_) || (!defined(BOTAN_IS_BEING_BUILT) && !defined(__clang_analyzer__))

namespace Botan {

/**
* Time (GeneralizedTime/UniversalTime)
*/
class BOTAN_PUBLIC_API(2, 0) ASN1_Time final : public ASN1_Object {
   public:
      /// DER encode a ASN1_Time
      void encode_into(DER_Encoder& to) const override;

      // Decode a BER encoded ASN1_Time
      void decode_from(BER_Decoder& from) override;

      /// Return an internal string representation of the time
      std::string to_string() const;

      /// Returns a human friendly string representation of no particular formatting
      std::string readable_string() const;

      /// Return if the time has been set somehow
      bool time_is_set() const;

      ///  Compare this time against another
      int32_t cmp(const ASN1_Time& other) const;

      /// Create an invalid ASN1_Time
      ASN1_Time() = default;

      /// Create a ASN1_Time from a time point
      explicit ASN1_Time(const std::chrono::system_clock::time_point& time);

      /// Create an ASN1_Time from seconds since epoch
      static ASN1_Time from_seconds_since_epoch(uint64_t seconds);

      /// Create an ASN1_Time from string
      BOTAN_FUTURE_EXPLICIT ASN1_Time(std::string_view t_spec);

      /// Create an ASN1_Time from string and a specified tagging (Utc or Generalized)
      ASN1_Time(std::string_view t_spec, ASN1_Type tag);

      /// Returns a STL timepoint object
      std::chrono::system_clock::time_point to_std_timepoint() const;

      /// Return time since epoch
      uint64_t time_since_epoch() const;

   private:
      void set_to(std::string_view t_spec, ASN1_Type type);
      bool passes_sanity_check() const;

      uint32_t m_year = 0;
      uint32_t m_month = 0;
      uint32_t m_day = 0;
      uint32_t m_hour = 0;
      uint32_t m_minute = 0;
      uint32_t m_second = 0;
      ASN1_Type m_tag = ASN1_Type::NoObject;
};

/*
* Comparison Operations
*/
BOTAN_PUBLIC_API(2, 0) bool operator==(const ASN1_Time& x, const ASN1_Time& y);
BOTAN_PUBLIC_API(2, 0) bool operator!=(const ASN1_Time& x, const ASN1_Time& y);
BOTAN_PUBLIC_API(2, 0) bool operator<=(const ASN1_Time& x, const ASN1_Time& y);
BOTAN_PUBLIC_API(2, 0) bool operator>=(const ASN1_Time& x, const ASN1_Time& y);
BOTAN_PUBLIC_API(2, 0) bool operator<(const ASN1_Time& x, const ASN1_Time& y);
BOTAN_PUBLIC_API(2, 0) bool operator>(const ASN1_Time& x, const ASN1_Time& y);

}  // namespace Botan
#endif

namespace Botan {

class BigInt;
class BER_Decoder;

/**
* Format ASN.1 data and call a virtual to format
*/
class BOTAN_PUBLIC_API(2, 4) ASN1_Formatter /* NOLINT(*-special-member-functions) */ {
   public:
      virtual ~ASN1_Formatter() = default;

      /**
      * @param print_context_specific if true, try to parse nested context specific data.
      * @param max_depth do not recurse more than this many times. If zero, recursion
      *        is unbounded.
      * @param require_der if true then non-canonical BER data is rejected
      */
      ASN1_Formatter(bool print_context_specific, size_t max_depth, bool require_der = false) :
            m_print_context_specific(print_context_specific), m_max_depth(max_depth), m_require_der(require_der) {}

      void print_to_stream(std::ostream& out, const uint8_t in[], size_t len) const;

      std::string print(const uint8_t in[], size_t len) const;

      template <typename Alloc>
      std::string print(const std::vector<uint8_t, Alloc>& vec) const {
         return print(vec.data(), vec.size());
      }

   protected:
      /**
      * This is called for each element
      */
      virtual std::string format(
         ASN1_Type type_tag, ASN1_Class class_tag, size_t level, size_t length, std::string_view value) const = 0;

      /**
      * This is called to format binary elements that we don't know how to
      * convert to a string. The result will be passed as value to format; the
      * tags are included as a hint to aid decoding.
      *
      * TODO(Botan4) change the vector to a span
      */
      virtual std::string format_bin(ASN1_Type type_tag,
                                     ASN1_Class class_tag,
                                     const std::vector<uint8_t>& vec) const = 0;

      /**
      * This is called to format integers
      */
      virtual std::string format_bn(const BigInt& bn) const = 0;

   private:
      void decode(std::ostream& output, BER_Decoder& decoder, size_t level) const;

      const bool m_print_context_specific;
      const size_t m_max_depth;
      const bool m_require_der;
};

/**
* Format ASN.1 data into human readable output. The exact form of the output for
* any particular input is not guaranteed and may change from release to release.
*/
class BOTAN_PUBLIC_API(2, 4) ASN1_Pretty_Printer final : public ASN1_Formatter {
   public:
      /**
      * @param print_limit strings larger than this are not printed
      * @param print_binary_limit binary strings larger than this are not printed
      * @param print_context_specific if true, try to parse nested context specific data.
      * @param initial_level the initial depth (0 or 1 are the only reasonable values)
      * @param value_column ASN.1 values are lined up at this column in output
      * @param max_depth do not recurse more than this many times. If zero, recursion
      *        is unbounded.
      * @param require_der if true then non-canonical BER data is rejected
      */
      explicit ASN1_Pretty_Printer(size_t print_limit = 4096,
                                   size_t print_binary_limit = 2048,
                                   bool print_context_specific = true,
                                   size_t initial_level = 0,
                                   size_t value_column = 60,
                                   size_t max_depth = 64,
                                   bool require_der = false) :
            ASN1_Formatter(print_context_specific, max_depth, require_der),
            m_print_limit(print_limit),
            m_print_binary_limit(print_binary_limit),
            m_initial_level(initial_level),
            m_value_column(value_column) {}

   private:
      std::string format(
         ASN1_Type type_tag, ASN1_Class class_tag, size_t level, size_t length, std::string_view value) const override;

      std::string format_bin(ASN1_Type type_tag, ASN1_Class class_tag, const std::vector<uint8_t>& vec) const override;

      std::string format_bn(const BigInt& bn) const override;

      const size_t m_print_limit;
      const size_t m_print_binary_limit;
      const size_t m_initial_level;
      const size_t m_value_column;
};

}  // namespace Botan

BOTAN_FUTURE_INTERNAL_HEADER(assert.h)

namespace Botan {

// NOLINTBEGIN(*-macro-usage)

/**
* Called when an assertion fails
* Throws an Exception object
*/
[[noreturn]] void BOTAN_PUBLIC_API(2, 0)
   assertion_failure(const char* expr_str, const char* assertion_made, const char* func, const char* file, int line);

/**
* Called when an invalid argument is used
* Throws Invalid_Argument
*/
[[noreturn]] void BOTAN_UNSTABLE_API throw_invalid_argument(const char* message, const char* func, const char* file);

#define BOTAN_ARG_CHECK(expr, msg)                               \
   /* NOLINTNEXTLINE(*-avoid-do-while) */                        \
   do {                                                          \
      /* NOLINTNEXTLINE(*-simplify-boolean-expr) */              \
      if(!(expr)) {                                              \
         /* NOLINTNEXTLINE(bugprone-lambda-function-name) */     \
         Botan::throw_invalid_argument(msg, __func__, __FILE__); \
      }                                                          \
   } while(0)

/**
* Called when an invalid state is encountered
* Throws Invalid_State
*/
[[noreturn]] void BOTAN_UNSTABLE_API throw_invalid_state(const char* message, const char* func, const char* file);

#define BOTAN_STATE_CHECK(expr)                                 \
   /* NOLINTNEXTLINE(*-avoid-do-while) */                       \
   do {                                                         \
      /* NOLINTNEXTLINE(*-simplify-boolean-expr) */             \
      if(!(expr)) {                                             \
         /* NOLINTNEXTLINE(bugprone-lambda-function-name) */    \
         Botan::throw_invalid_state(#expr, __func__, __FILE__); \
      }                                                         \
   } while(0)

/**
* Make an assertion
*/
#define BOTAN_ASSERT(expr, assertion_made)                                              \
   /* NOLINTNEXTLINE(*-avoid-do-while) */                                               \
   do {                                                                                 \
      /* NOLINTNEXTLINE(*-simplify-boolean-expr) */                                     \
      if(!(expr)) {                                                                     \
         /* NOLINTNEXTLINE(bugprone-lambda-function-name) */                            \
         Botan::assertion_failure(#expr, assertion_made, __func__, __FILE__, __LINE__); \
      }                                                                                 \
   } while(0)

/**
* Make an assertion
*/
#define BOTAN_ASSERT_NOMSG(expr)                                            \
   /* NOLINTNEXTLINE(*-avoid-do-while) */                                   \
   do {                                                                     \
      /* NOLINTNEXTLINE(*-simplify-boolean-expr) */                         \
      if(!(expr)) {                                                         \
         /* NOLINTNEXTLINE(bugprone-lambda-function-name) */                \
         Botan::assertion_failure(#expr, "", __func__, __FILE__, __LINE__); \
      }                                                                     \
   } while(0)

/**
* Assert that value1 == value2
*/
#define BOTAN_ASSERT_EQUAL(expr1, expr2, assertion_made)                                               \
   /* NOLINTNEXTLINE(*-avoid-do-while) */                                                              \
   do {                                                                                                \
      /* NOLINTNEXTLINE(*-simplify-boolean-expr) */                                                    \
      if((expr1) != (expr2)) {                                                                         \
         /* NOLINTNEXTLINE(bugprone-lambda-function-name) */                                           \
         Botan::assertion_failure(#expr1 " == " #expr2, assertion_made, __func__, __FILE__, __LINE__); \
      }                                                                                                \
   } while(0)

/**
* Assert that expr1 (if true) implies expr2 is also true
*/
#define BOTAN_ASSERT_IMPLICATION(expr1, expr2, msg)                                              \
   /* NOLINTNEXTLINE(*-avoid-do-while) */                                                        \
   do {                                                                                          \
      /* NOLINTNEXTLINE(*-simplify-boolean-expr) */                                              \
      if((expr1) && !(expr2)) {                                                                  \
         /* NOLINTNEXTLINE(bugprone-lambda-function-name) */                                     \
         Botan::assertion_failure(#expr1 " implies " #expr2, msg, __func__, __FILE__, __LINE__); \
      }                                                                                          \
   } while(0)

/**
* Assert that a pointer is not null
*/
#define BOTAN_ASSERT_NONNULL(ptr)                                                         \
   /* NOLINTNEXTLINE(*-avoid-do-while) */                                                 \
   do {                                                                                   \
      if((ptr) == nullptr) {                                                              \
         /* NOLINTNEXTLINE(bugprone-lambda-function-name) */                              \
         Botan::assertion_failure(#ptr " is not null", "", __func__, __FILE__, __LINE__); \
      }                                                                                   \
   } while(0)

#if defined(BOTAN_ENABLE_DEBUG_ASSERTS)

   #define BOTAN_DEBUG_ASSERT(expr) BOTAN_ASSERT_NOMSG(expr)

#else

   #define BOTAN_DEBUG_ASSERT(expr)       \
      do { /* NOLINT(*-avoid-do-while) */ \
      } while(0)

#endif

/**
* Mark variable as unused.
*
* Takes any number of arguments and marks all as unused, for instance
* BOTAN_UNUSED(a); or BOTAN_UNUSED(x, y, z);
*/
template <typename... T>
constexpr void ignore_params([[maybe_unused]] const T&... args) {}

#define BOTAN_UNUSED Botan::ignore_params

/*
* Define Botan::assert_unreachable and BOTAN_ASSERT_UNREACHABLE
*
* This is intended to be used in the same situations as `std::unreachable()`;
* a codepath that (should not) be reachable but where the compiler cannot
* tell that it is unreachable.
*
* Unlike `std::unreachable()`, or equivalent compiler builtins like GCC's
* `__builtin_unreachable`, this function is not UB. By default it will
* throw an exception. If `BOTAN_TERMINATE_ON_ASSERTS` is defined, it will
* instead print a message to stderr and abort.
*
* Due to this difference, and the fact that it is not inlined, calling
* this is significantly more costly than using `std::unreachable`.
*/
[[noreturn]] void BOTAN_UNSTABLE_API assert_unreachable(const char* file, int line);

#define BOTAN_ASSERT_UNREACHABLE() Botan::assert_unreachable(__FILE__, __LINE__)

// NOLINTEND(*-macro-usage)

}  // namespace Botan

namespace Botan {

template <typename T0 = void, typename... Ts>
struct all_same {
      static constexpr bool value = (std::is_same_v<T0, Ts> && ... && true);
};

template <typename... Ts>
static constexpr bool all_same_v = all_same<Ts...>::value;

namespace detail {

/**
 * Helper type to indicate that a certain type should be automatically
 * detected based on the context.
 */
struct AutoDetect {
      constexpr AutoDetect() = delete;
};

}  // namespace detail

namespace concepts {

// TODO: C++20 provides concepts like std::ranges::range or ::sized_range
//       but at the time of this writing clang had not caught up on all
//       platforms. E.g. clang 14 on Xcode does not support ranges properly.

template <typename IterT, typename ContainerT>
concept container_iterator =
   std::same_as<IterT, typename ContainerT::iterator> || std::same_as<IterT, typename ContainerT::const_iterator>;

template <typename PtrT, typename ContainerT>
concept container_pointer =
   std::same_as<PtrT, typename ContainerT::pointer> || std::same_as<PtrT, typename ContainerT::const_pointer>;

template <typename T>
concept container = requires(T a) {
   { a.begin() } -> container_iterator<T>;
   { a.end() } -> container_iterator<T>;
   { a.cbegin() } -> container_iterator<T>;
   { a.cend() } -> container_iterator<T>;
   { a.size() } -> std::same_as<typename T::size_type>;
   typename T::value_type;
};

template <typename T>
concept contiguous_container = container<T> && requires(T a) {
   { a.data() } -> container_pointer<T>;
};

template <typename T>
concept has_empty = requires(T a) {
   { a.empty() } -> std::same_as<bool>;
};

template <typename T>
concept resizable_container = container<T> && requires(T& c, typename T::size_type s) {
   T(s);
   c.resize(s);
};

template <typename T>
concept reservable_container = container<T> && requires(T& c, typename T::size_type s) { c.reserve(s); };

template <typename T>
concept resizable_byte_buffer =
   contiguous_container<T> && resizable_container<T> && std::same_as<typename T::value_type, uint8_t>;

}  // namespace concepts

}  // namespace Botan

#if !defined(BOTAN_IS_BEING_BUILT) && !defined(BOTAN_DISABLE_DEPRECATED_FEATURES)
   // TODO(Botan4) remove this
   #include <deque>
#endif

namespace Botan {

template <typename T>
#if !defined(_ITERATOR_DEBUG_LEVEL) || _ITERATOR_DEBUG_LEVEL == 0
/*
  * Check exists to prevent someone from doing something that will
  * probably crash anyway (like secure_vector<non_POD_t> where ~non_POD_t
  * deletes a member pointer which was zeroed before it ran).
  * MSVC in debug mode uses non-integral proxy types in container types
  * like std::vector, thus we disable the check there.
 */
   requires std::is_integral_v<T> || std::is_enum_v<T>
#endif
class secure_allocator {

   public:
      typedef T value_type;
      typedef std::size_t size_type;

      secure_allocator() noexcept = default;
      secure_allocator(const secure_allocator&) noexcept = default;
      secure_allocator& operator=(const secure_allocator&) noexcept = default;
      secure_allocator(secure_allocator&&) noexcept = default;
      secure_allocator& operator=(secure_allocator&&) noexcept = default;

      ~secure_allocator() noexcept = default;

      template <typename U>
      explicit secure_allocator(const secure_allocator<U>& /*other*/) noexcept {}

      T* allocate(std::size_t n) { return static_cast<T*>(allocate_memory(n, sizeof(T))); }

      void deallocate(T* p, std::size_t n) { deallocate_memory(p, n, sizeof(T)); }
};

template <typename T, typename U>
inline bool operator==(const secure_allocator<T>& /*a*/, const secure_allocator<U>& /*b*/) {
   return true;
}

template <typename T, typename U>
inline bool operator!=(const secure_allocator<T>& /*a*/, const secure_allocator<U>& /*b*/) {
   return false;
}

template <typename T>
using secure_vector = std::vector<T, secure_allocator<T>>;

#if !defined(BOTAN_IS_BEING_BUILT) && !defined(BOTAN_DISABLE_DEPRECATED_FEATURES)
template <typename T>
using secure_deque = std::deque<T, secure_allocator<T>>;
#endif

// For better compatibility with 1.10 API
template <typename T>
using SecureVector = secure_vector<T>;

template <typename T>
secure_vector<T> lock(const std::vector<T>& in) {
   return secure_vector<T>(in.begin(), in.end());
}

template <typename T>
std::vector<T> unlock(const secure_vector<T>& in) {
   return std::vector<T>(in.begin(), in.end());
}

// TODO(Botan4) remove these += operators entirely

template <typename T, typename Alloc, typename Alloc2>
std::vector<T, Alloc>& operator+=(std::vector<T, Alloc>& out, const std::vector<T, Alloc2>& in) {
   out.insert(out.end(), in.begin(), in.end());
   return out;
}

template <typename T, typename Alloc>
std::vector<T, Alloc>& operator+=(std::vector<T, Alloc>& out, std::span<const T> in) {
   out.insert(out.end(), in.begin(), in.end());
   return out;
}

template <typename T, typename Alloc>
std::vector<T, Alloc>& operator+=(std::vector<T, Alloc>& out, T in) {
   out.push_back(in);
   return out;
}

template <typename T, typename Alloc, typename L>
std::vector<T, Alloc>& operator+=(std::vector<T, Alloc>& out, const std::pair<const T*, L>& in) {
   out.insert(out.end(), in.first, in.first + in.second);
   return out;
}

template <typename T, typename Alloc, typename L>
std::vector<T, Alloc>& operator+=(std::vector<T, Alloc>& out, const std::pair<T*, L>& in) {
   out.insert(out.end(), in.first, in.first + in.second);
   return out;
}

/**
* Zeroise the values; length remains unchanged
*
* Note this is not intended for cases where the compiler might elide
* the writes as being without side-effects; use secure_scrub_memory
* for that.
*
* TODO(Botan4): make these not-inlined and only for secure_vector, eg declare
*  void zeroize(secure_vector<uint8_t>& v);
*  void zeroize(secure_vector<uint16_t>& v);
*  void zeroize(secure_vector<uint32_t>& v);
*  void zeroize(secure_vector<uint64_t>& v);
*
* @param vec the vector to zeroise
*/
template <typename T, typename Alloc>
void zeroise(std::vector<T, Alloc>& vec) {
   for(size_t i = 0; i != vec.size(); ++i) {
      vec[i] = static_cast<T>(0);
   }
}

/**
* Zeroise the values then free the memory
*
* TODO(Botan4): make these not-inlined and only for secure_vector, eg declare
*  void zap(secure_vector<uint8_t>& v);
*  void zap(secure_vector<uint16_t>& v);
*  void zap(secure_vector<uint32_t>& v);
*  void zap(secure_vector<uint64_t>& v);
*
* [And maybe rename as well]
*
* @param vec the vector to zeroise and free
*/
template <typename T, typename Alloc>
void zap(std::vector<T, Alloc>& vec) {
   zeroise(vec);
   vec.clear();
   vec.shrink_to_fit();
}

}  // namespace Botan


/*
* We only include <chrono> in downstream applications to avoid
* breaking semver wrt RandomNumberGenerator::reseed. Within the
* library we avoid it because it slows down compilation significantly.
*
* TODO(Botan4): remove this entirely
*/
#if !defined(BOTAN_IS_BEING_BUILT)
   #include <chrono>
#endif

namespace Botan {

class Entropy_Sources;

/**
* An interface to a cryptographic random number generator
*/
class BOTAN_PUBLIC_API(2, 0) RandomNumberGenerator {
   public:
      /**
      * Userspace RNGs like HMAC_DRBG will reseed after a specified number
      * of outputs are generated. Set to zero to disable automatic reseeding.
      */
      static constexpr size_t DefaultReseedInterval = 1024;

      /**
      * Number of entropy bits polled for reseeding userspace RNGs like HMAC_DRBG
      */
      static constexpr size_t DefaultPollBits = 256;

      virtual ~RandomNumberGenerator() = default;

      RandomNumberGenerator() = default;

      /*
      * Never copy a RNG, create a new one
      */
      RandomNumberGenerator(const RandomNumberGenerator& rng) = delete;
      RandomNumberGenerator& operator=(const RandomNumberGenerator& rng) = delete;

      RandomNumberGenerator(RandomNumberGenerator&& rng) = default;
      RandomNumberGenerator& operator=(RandomNumberGenerator&& rng) = default;

      /**
      * Randomize a byte array.
      *
      * May block shortly if e.g. the RNG is not yet initialized
      * or a retry because of insufficient entropy is needed.
      *
      * @param output the byte array to hold the random output.
      * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
      * @throws Exception if the RNG fails
      */
      void randomize(std::span<uint8_t> output) { this->fill_bytes_with_input(output, {}); }

      void randomize(uint8_t output[], size_t length) { this->randomize(std::span(output, length)); }

      /**
      * Returns false if it is known that this RNG object is not able to accept
      * externally provided inputs (via add_entropy, randomize_with_input, etc).
      * In this case, any such provided inputs are ignored.
      *
      * If this function returns true, then inputs may or may not be accepted.
      */
      virtual bool accepts_input() const = 0;

      /**
      * Incorporate some additional data into the RNG state. For
      * example adding nonces or timestamps from a peer's protocol
      * message can help hedge against VM state rollback attacks.
      * A few RNG types do not accept any externally provided input,
      * in which case this function is a no-op.
      *
      * @param input a byte array containing the entropy to be added
      * @throws Exception may throw if the RNG accepts input, but adding the entropy failed.
      */
      void add_entropy(std::span<const uint8_t> input) { this->fill_bytes_with_input({}, input); }

      void add_entropy(const uint8_t input[], size_t length) { this->add_entropy(std::span(input, length)); }

      /**
      * Incorporate some additional data into the RNG state.
      */
      template <typename T>
         requires std::is_standard_layout_v<T> && std::is_trivial_v<T>
      void add_entropy_T(const T& t) {
         this->add_entropy(reinterpret_cast<const uint8_t*>(&t), sizeof(T));
      }

      /**
      * Incorporate entropy into the RNG state then produce output.
      * Some RNG types implement this using a single operation, default
      * calls add_entropy + randomize in sequence.
      *
      * Use this to further bind the outputs to your current
      * process/protocol state. For instance if generating a new key
      * for use in a session, include a session ID or other such
      * value. See NIST SP 800-90 A, B, C series for more ideas.
      *
      * @param output buffer to hold the random output
      * @param input entropy buffer to incorporate
      * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
      * @throws Exception if the RNG fails
      * @throws Exception may throw if the RNG accepts input, but adding the entropy failed.
      */
      void randomize_with_input(std::span<uint8_t> output, std::span<const uint8_t> input) {
         this->fill_bytes_with_input(output, input);
      }

      void randomize_with_input(uint8_t output[], size_t output_len, const uint8_t input[], size_t input_len) {
         this->randomize_with_input(std::span(output, output_len), std::span(input, input_len));
      }

      /**
      * This calls `randomize_with_input` using some timestamps as extra input.
      *
      * For a stateful RNG using non-random but potentially unique data the
      * extra input can help protect against problems with fork, VM state
      * rollback, or other cases where somehow an RNG state is duplicated. If
      * both of the duplicated RNG states later incorporate a timestamp (and the
      * timestamps don't themselves repeat), their outputs will diverge.
      *
      * @param output buffer to hold the random output
      * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
      * @throws Exception if the RNG fails
      * @throws Exception may throw if the RNG accepts input, but adding the entropy failed.
      */
      void randomize_with_ts_input(std::span<uint8_t> output);

      void randomize_with_ts_input(uint8_t output[], size_t output_len) {
         this->randomize_with_ts_input(std::span(output, output_len));
      }

      /**
      * @return the name of this RNG type
      */
      virtual std::string name() const = 0;

      /**
      * Clear all internally held values of this RNG
      * @post is_seeded() == false if the RNG has an internal state that can be cleared.
      */
      virtual void clear() = 0;

      /**
      * Check whether this RNG is seeded.
      * @return true if this RNG was already seeded, false otherwise.
      */
      virtual bool is_seeded() const = 0;

      /**
      * Poll provided sources for up to poll_bits bits of entropy.
      * Returns estimate of the number of bits collected.
      * Sets the seeded state to true if enough entropy was added.
      *
      * @throws Exception if RNG accepts input but reseeding failed.
      */
      size_t reseed_from(Entropy_Sources& srcs, size_t poll_bits = RandomNumberGenerator::DefaultPollBits) {
         return reseed_from_sources(srcs, poll_bits);
      }

      /**
      * Reseed by reading specified bits from the RNG
      *
      * Sets the seeded state to true if enough entropy was added.
      *
      * @throws Exception if RNG accepts input but reseeding failed.
      */
      void reseed_from(RandomNumberGenerator& rng, size_t poll_bits = RandomNumberGenerator::DefaultPollBits) {
         return reseed_from_rng(rng, poll_bits);
      }

      // Some utility functions built on the interface above:

      /**
      * Fill a given byte container with @p bytes random bytes
      *
      * @todo deprecate this overload (in favor of randomize())
      *
      * @param  v     the container to be filled with @p bytes random bytes
      * @throws Exception if RNG fails
      */
      void random_vec(std::span<uint8_t> v) { this->randomize(v); }

      /**
      * Resize a given byte container to @p bytes and fill it with random bytes
      *
      * @tparam T     the desired byte container type (e.g std::vector<uint8_t>)
      * @param  v     the container to be filled with @p bytes random bytes
      * @param  bytes number of random bytes to initialize the container with
      * @throws Exception if RNG or memory allocation fails
      */
      template <concepts::resizable_byte_buffer T>
      void random_vec(T& v, size_t bytes) {
         v.resize(bytes);
         random_vec(v);
      }

      /**
      * Create some byte container type and fill it with some random @p bytes.
      *
      * @tparam T     the desired byte container type (e.g std::vector<uint8_t>)
      * @param  bytes number of random bytes to initialize the container with
      * @return       a container of type T with @p bytes random bytes
      * @throws Exception if RNG or memory allocation fails
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
         requires std::default_initializable<T>
      T random_vec(size_t bytes) {
         T result;
         random_vec(result, bytes);
         return result;
      }

      /**
       * Create a std::array of @p bytes random bytes
       */
      template <size_t bytes>
      std::array<uint8_t, bytes> random_array() {
         std::array<uint8_t, bytes> result{};
         random_vec(result);
         return result;
      }

      /**
      * Return a random byte
      * @return random byte
      * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
      * @throws Exception if the RNG fails
      */
      uint8_t next_byte() {
         uint8_t b = 0;
         this->fill_bytes_with_input(std::span(&b, 1), {});
         return b;
      }

      /**
      * @return a random byte that is greater than zero
      * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
      * @throws Exception if the RNG fails
      */
      uint8_t next_nonzero_byte() {
         uint8_t b = this->next_byte();
         while(b == 0) {
            b = this->next_byte();
         }
         return b;
      }

      /**
      * Reseed by reading specified bits from the RNG
      *
      * Sets the seeded state to true if enough entropy was added.
      *
      * @throws Exception if RNG accepts input but reseeding failed.
      */
      virtual void reseed_from_rng(RandomNumberGenerator& rng,
                                   size_t poll_bits = RandomNumberGenerator::DefaultPollBits);

#if !defined(BOTAN_IS_BEING_BUILT)
      /**
      * Default poll timeout
      */
      static constexpr auto DefaultPollTimeout = std::chrono::milliseconds(50);

      /**
       * Poll provided sources for up to poll_bits bits of entropy.
       * Returns estimate of the number of bits collected.
       *
       * Sets the seeded state to true if enough entropy was added.
       *
       * TODO(Botan4) remove this function
       */
      BOTAN_DEPRECATED("Use reseed_from_sources")
      inline size_t reseed(Entropy_Sources& srcs,
                           size_t poll_bits = RandomNumberGenerator::DefaultPollBits,
                           std::chrono::milliseconds /*unused_timeout*/ = DefaultPollTimeout) {
         return reseed_from(srcs, poll_bits);
      }
#endif

   protected:
      /**
      * Poll provided sources for up to poll_bits bits of entropy.
      * Returns estimate of the number of bits collected.
      * Sets the seeded state to true if enough entropy was added.
      *
      * @throws Exception if RNG accepts input but reseeding failed.
      */
      virtual size_t reseed_from_sources(Entropy_Sources& srcs,
                                         size_t poll_bits = RandomNumberGenerator::DefaultPollBits);

      /**
      * Generic interface to provide entropy to a concrete implementation and to
      * fill a given buffer with random output. Both @p output and @p input may
      * be empty and should be ignored in that case. If both buffers are
      * non-empty implementations should typically first apply the @p input data
      * and then generate random data into @p output.
      *
      * This method must be implemented by all RandomNumberGenerator sub-classes.
      *
      * @param output  Byte buffer to write random bytes into. Implementations
      *                should not read from this buffer.
      * @param input   Byte buffer that may contain bytes to be incorporated in
      *                the RNG's internal state. Implementations may choose to
      *                ignore the bytes in this buffer.
      */
      virtual void fill_bytes_with_input(std::span<uint8_t> output, std::span<const uint8_t> input) = 0;
};

/**
* Convenience typedef
*/
typedef RandomNumberGenerator RNG;

/**
* Hardware_RNG exists to tag hardware RNG types (PKCS11_RNG, TPM_RNG, Processor_RNG)
*/
class BOTAN_PUBLIC_API(2, 0) Hardware_RNG : public RandomNumberGenerator {
   public:
      void clear() final { /* no way to clear state of hardware RNG */
      }
};

/**
* Null/stub RNG - fails if you try to use it for anything
* This is not generally useful except for in certain tests
*/
class BOTAN_PUBLIC_API(2, 0) Null_RNG final : public RandomNumberGenerator {
   public:
      bool is_seeded() const override { return false; }

      bool accepts_input() const override { return false; }

      void clear() override {}

      std::string name() const override { return "Null_RNG"; }

   private:
      void fill_bytes_with_input(std::span<uint8_t> output, std::span<const uint8_t> /* ignored */) override;
};

}  // namespace Botan

namespace Botan {

class Stateful_RNG;

/**
* A userspace PRNG
*/
class BOTAN_PUBLIC_API(2, 0) AutoSeeded_RNG final : public RandomNumberGenerator {
   public:
      bool is_seeded() const override;

      bool accepts_input() const override { return true; }

      /**
      * Mark state as requiring a reseed on next use
      */
      void force_reseed();

      size_t reseed_from_sources(Entropy_Sources& srcs,
                                 size_t poll_bits = RandomNumberGenerator::DefaultPollBits) override;

      std::string name() const override;

      void clear() override;

      /**
      * Uses the system RNG (if available) or else a default group of
      * entropy sources (all other systems) to gather seed material.
      *
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed
      */
      BOTAN_FUTURE_EXPLICIT AutoSeeded_RNG(size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval);

      /**
      * Create an AutoSeeded_RNG which will get seed material from some other
      * RNG instance. For example you could provide a reference to the system
      * RNG or a hardware RNG.
      *
      * @param underlying_rng is a reference to some RNG which will be used
      * to perform the periodic reseeding
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed
      */
      BOTAN_FUTURE_EXPLICIT AutoSeeded_RNG(RandomNumberGenerator& underlying_rng,
                                           size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval);

      /**
      * Create an AutoSeeded_RNG which will get seed material from a set of
      * entropy sources.
      *
      * @param entropy_sources will be polled to perform reseeding periodically
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed
      */
      BOTAN_FUTURE_EXPLICIT AutoSeeded_RNG(Entropy_Sources& entropy_sources,
                                           size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval);

      /**
      * Create an AutoSeeded_RNG which will get seed material from both an
      * underlying RNG and a set of entropy sources.
      *
      * @param underlying_rng is a reference to some RNG which will be used
      * to perform the periodic reseeding
      * @param entropy_sources will be polled to perform reseeding periodically
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed
      */
      AutoSeeded_RNG(RandomNumberGenerator& underlying_rng,
                     Entropy_Sources& entropy_sources,
                     size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval);

      AutoSeeded_RNG(const AutoSeeded_RNG& other) = delete;
      AutoSeeded_RNG(AutoSeeded_RNG&& other) noexcept;
      AutoSeeded_RNG& operator=(const AutoSeeded_RNG& other) = delete;
      AutoSeeded_RNG& operator=(AutoSeeded_RNG&& other) = delete;

      ~AutoSeeded_RNG() override;

   private:
      void fill_bytes_with_input(std::span<uint8_t> out, std::span<const uint8_t> in) override;

   private:
      std::unique_ptr<Stateful_RNG> m_rng;
};

}  // namespace Botan

namespace Botan {

/**
* Perform base64 encoding
* @param output an array of at least base64_encode_max_output bytes
* @param input is some binary data
* @param input_length length of input in bytes
* @param input_consumed is an output parameter which says how many
*        bytes of input were actually consumed. If less than
*        input_length, then the range input[consumed:length]
*        should be passed in later along with more input.
* @param final_inputs true iff this is the last input, in which case
         padding chars will be applied if needed
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0)
   base64_encode(char output[], const uint8_t input[], size_t input_length, size_t& input_consumed, bool final_inputs);

/**
* Perform base64 encoding
* @param input some input
* @param input_length length of input in bytes
* @return base64adecimal representation of input
*/
std::string BOTAN_PUBLIC_API(2, 0) base64_encode(const uint8_t input[], size_t input_length);

/**
* Perform base64 encoding
* @param input some input
* @return base64adecimal representation of input
*/
inline std::string base64_encode(std::span<const uint8_t> input) {
   return base64_encode(input.data(), input.size());
}

/**
* Perform base64 decoding
* @param output an array of at least base64_decode_max_output bytes
* @param input some base64 input
* @param input_length length of input in bytes
* @param input_consumed is an output parameter which says how many
*        bytes of input were actually consumed. If less than
*        input_length, then the range input[consumed:length]
*        should be passed in later along with more input.
* @param final_inputs true iff this is the last input, in which case
         padding is allowed
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0) base64_decode(uint8_t output[],
                                            const char input[],
                                            size_t input_length,
                                            size_t& input_consumed,
                                            bool final_inputs,
                                            bool ignore_ws = true);

/**
* Perform base64 decoding
* @param output an array of at least base64_decode_max_output bytes
* @param input some base64 input
* @param input_length length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0)
   base64_decode(uint8_t output[], const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param output an array of at least base64_decode_max_output bytes
* @param input some base64 input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(3, 0) base64_decode(uint8_t output[], std::string_view input, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param output a contiguous byte buffer of at least base64_decode_max_output bytes
* @param input some base64 input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(3, 0) base64_decode(std::span<uint8_t> output, std::string_view input, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param input some base64 input
* @param input_length the length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded base64 output
*/
secure_vector<uint8_t> BOTAN_PUBLIC_API(2, 0)
   base64_decode(const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param input some base64 input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded base64 output
*/
secure_vector<uint8_t> BOTAN_PUBLIC_API(3, 0) base64_decode(std::string_view input, bool ignore_ws = true);

/**
* Calculate the size of output buffer for base64_encode
* @param input_length the length of input in bytes
* @return the size of output buffer in bytes
*/
size_t BOTAN_PUBLIC_API(2, 1) base64_encode_max_output(size_t input_length);

/**
* Calculate the size of output buffer for base64_decode
* @param input_length the length of input in bytes
* @return the size of output buffer in bytes
*/
size_t BOTAN_PUBLIC_API(2, 1) base64_decode_max_output(size_t input_length);

}  // namespace Botan

namespace Botan {

class BigInt;
class DataSource;

/**
* BER Decoding Object
*/
class BOTAN_PUBLIC_API(2, 0) BER_Decoder final {
   public:
      /**
      * Controls what encoding rules the decoder accepts.
      */
      class BOTAN_PUBLIC_API(3, 12) Limits final {
         public:
            /**
            * Accept only DER encodings
            */
            static Limits DER() { return Limits(false, 0); }

            /**
            * Accept non-canonical BER encodings.
            *
            * @param max_nested_indef maximum number of nested indefinite-length encodings accepted
            */
            static Limits BER(size_t max_nested_indef = 16) { return Limits(true, max_nested_indef); }

            bool allow_ber_encoding() const { return m_allow_ber; }

            bool require_der_encoding() const { return !allow_ber_encoding(); }

            size_t max_nested_indefinite_length() const { return m_max_nested_indef; }

            bool operator==(const Limits&) const = default;

         private:
            Limits(bool allow_ber, size_t max_nested_indef) :
                  m_allow_ber(allow_ber), m_max_nested_indef(max_nested_indef) {}

            bool m_allow_ber;
            size_t m_max_nested_indef;
      };

      /**
      * Set up to BER decode the data in buf of length len
      */
      BOTAN_DEPRECATED("Use BER_Decoder(span) constructor")
      BER_Decoder(const uint8_t buf[], size_t len, Limits limits = Limits::BER()) :
            BER_Decoder(std::span{buf, len}, limits) {}

      /**
      * Set up to BER decode the data in buf
      */
      explicit BER_Decoder(std::span<const uint8_t> buf, Limits limits = Limits::BER());

      /**
      * Set up to BER decode the data in src
      */
      explicit BER_Decoder(DataSource& src, Limits limits = Limits::BER());

      /**
      * Set up to BER decode the data in obj
      */
      BOTAN_FUTURE_EXPLICIT BER_Decoder(const BER_Object& obj, Limits limits = Limits::BER()) :
            BER_Decoder(obj.data(), limits) {}

      /**
      * Set up to BER decode the data in obj
      * TODO(Botan4) remove this?
      */
      BOTAN_FUTURE_EXPLICIT BER_Decoder(BER_Object&& obj) : BER_Decoder(std::move(obj), nullptr) {}

      BER_Decoder(const BER_Decoder& other) = delete;
      BER_Decoder(BER_Decoder&& other) noexcept;

      BER_Decoder& operator=(const BER_Decoder&) = delete;
      BER_Decoder& operator=(BER_Decoder&&) noexcept;

      /**
      * Returns the limits currently applied to this decoder
      */
      Limits limits() const { return m_limits; }

      /**
      * Get the next object in the data stream.
      * If EOF, returns an object with type NO_OBJECT.
      */
      BER_Object get_next_object();

      BER_Decoder& get_next(BER_Object& ber) {
         ber = get_next_object();
         return (*this);
      }

      /**
      * Peek at the next object without removing it from the stream
      *
      * If an object has been pushed, then it returns that object.
      * Otherwise it reads the next object and pushes it. Thus, a you
      * call peek_next_object followed by push_back without a
      * subsequent read, it will fail.
      */
      const BER_Object& peek_next_object();

      /**
      * Push an object back onto the stream. Throws if another
      * object was previously pushed and has not been subsequently
      * read out.
      */
      void push_back(const BER_Object& obj);

      /**
      * Push an object back onto the stream. Throws if another
      * object was previously pushed and has not been subsequently
      * read out.
      */
      void push_back(BER_Object&& obj);

      /**
      * Return true if there is at least one more item remaining
      */
      bool more_items() const;

      /**
      * Verify the stream is concluded, throws otherwise.
      * Returns (*this)
      */
      BER_Decoder& verify_end();

      /**
      * Verify the stream is concluded, throws otherwise.
      * Returns (*this)
      */
      BER_Decoder& verify_end(std::string_view err_msg);

      /**
      * Discard any data that remains unread
      * Returns (*this)
      */
      BER_Decoder& discard_remaining();

      BER_Decoder start_cons(ASN1_Type type_tag, ASN1_Class class_tag);

      BER_Decoder start_sequence() { return start_cons(ASN1_Type::Sequence, ASN1_Class::Universal); }

      BER_Decoder start_set() { return start_cons(ASN1_Type::Set, ASN1_Class::Universal); }

      BER_Decoder start_context_specific(uint32_t tag) {
         return start_cons(ASN1_Type(tag), ASN1_Class::ContextSpecific);
      }

      BER_Decoder start_explicit_context_specific(uint32_t tag) {
         return start_cons(ASN1_Type(tag), ASN1_Class::ExplicitContextSpecific);
      }

      /**
      * Finish decoding a constructed data, throws if any data remains.
      * Returns the parent of *this (ie the object on which start_cons was called).
      */
      BER_Decoder& end_cons();

      /**
      * Get next object and copy value to POD type
      * Asserts value length is equal to POD type sizeof.
      * Asserts Type tag and optional Class tag according to parameters.
      * Copy value to POD type (struct, union, C-style array, std::array, etc.).
      * @param out POD type reference where to copy object value
      * @param type_tag ASN1_Type enum to assert type on object read
      * @param class_tag ASN1_Type enum to assert class on object read (default: CONTEXT_SPECIFIC)
      * @return this reference
      */
      template <typename T>
      BER_Decoder& get_next_value(T& out, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific)
         requires std::is_standard_layout_v<T> && std::is_trivial_v<T>
      {
         const BER_Object obj = get_next_value(sizeof(T), type_tag, class_tag);

         std::memcpy(reinterpret_cast<uint8_t*>(&out), obj.bits(), obj.length());

         return (*this);
      }

      /*
      * Save all the bytes remaining in the source
      */
      template <typename Alloc>
      BER_Decoder& raw_bytes(std::vector<uint8_t, Alloc>& out) {
         out.clear();
         for(;;) {
            if(auto next = this->read_next_byte()) {
               out.push_back(*next);
            } else {
               break;
            }
         }
         return (*this);
      }

      BER_Decoder& decode_null();

      /**
      * Decode a BER encoded BOOLEAN
      */
      BER_Decoder& decode(bool& out) { return decode(out, ASN1_Type::Boolean, ASN1_Class::Universal); }

      /*
      * Decode a small BER encoded INTEGER
      */
      BER_Decoder& decode(size_t& out) { return decode(out, ASN1_Type::Integer, ASN1_Class::Universal); }

      /*
      * Decode a BER encoded INTEGER
      */
      BER_Decoder& decode(BigInt& out) { return decode(out, ASN1_Type::Integer, ASN1_Class::Universal); }

      std::vector<uint8_t> get_next_octet_string() {
         std::vector<uint8_t> out_vec;
         decode(out_vec, ASN1_Type::OctetString);
         return out_vec;
      }

      /*
      * BER decode a BIT STRING or OCTET STRING
      */
      template <typename Alloc>
      BER_Decoder& decode(std::vector<uint8_t, Alloc>& out, ASN1_Type real_type) {
         return decode(out, real_type, real_type, ASN1_Class::Universal);
      }

      BER_Decoder& decode(bool& v, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      BER_Decoder& decode(size_t& v, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      BER_Decoder& decode(BigInt& v, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      BER_Decoder& decode(std::vector<uint8_t>& v,
                          ASN1_Type real_type,
                          ASN1_Type type_tag,
                          ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      BER_Decoder& decode(secure_vector<uint8_t>& v,
                          ASN1_Type real_type,
                          ASN1_Type type_tag,
                          ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      BER_Decoder& decode(ASN1_Object& obj,
                          ASN1_Type type_tag = ASN1_Type::NoObject,
                          ASN1_Class class_tag = ASN1_Class::NoObject);

      /**
      * Decode an integer value which is typed as an octet string
      */
      BER_Decoder& decode_octet_string_bigint(BigInt& b);

      uint64_t decode_constrained_integer(ASN1_Type type_tag, ASN1_Class class_tag, size_t T_bytes);

      template <typename T>
      BER_Decoder& decode_integer_type(T& out) {
         return decode_integer_type<T>(out, ASN1_Type::Integer, ASN1_Class::Universal);
      }

      template <typename T>
      BER_Decoder& decode_integer_type(T& out, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific) {
         out = static_cast<T>(decode_constrained_integer(type_tag, class_tag, sizeof(out)));
         return (*this);
      }

      template <typename T>
      BER_Decoder& decode_optional(T& out, ASN1_Type type_tag, ASN1_Class class_tag, const T& default_value = T()) {
         std::optional<T> optval;
         this->decode_optional(optval, type_tag, class_tag);
         out = optval ? *optval : default_value;
         return (*this);
      }

      template <typename T>
      BER_Decoder& decode_optional(std::optional<T>& out, ASN1_Type type_tag, ASN1_Class class_tag);

      template <typename T>
      BER_Decoder& decode_optional_implicit(T& out,
                                            ASN1_Type type_tag,
                                            ASN1_Class class_tag,
                                            ASN1_Type real_type,
                                            ASN1_Class real_class,
                                            const T& default_value = T());

      template <typename T>
      BER_Decoder& decode_list(std::vector<T>& out,
                               ASN1_Type type_tag = ASN1_Type::Sequence,
                               ASN1_Class class_tag = ASN1_Class::Universal);

      template <typename T>
      bool decode_optional_list(std::vector<T>& out,
                                ASN1_Type type_tag = ASN1_Type::Sequence,
                                ASN1_Class class_tag = ASN1_Class::Universal);

      template <typename T>
      BER_Decoder& decode_and_check(const T& expected, std::string_view error_msg) {
         T actual;
         decode(actual);

         if(actual != expected) {
            throw Decoding_Error(error_msg);
         }

         return (*this);
      }

      /*
      * Decode an OPTIONAL string type
      */
      template <typename Alloc>
      BER_Decoder& decode_optional_string(std::vector<uint8_t, Alloc>& out,
                                          ASN1_Type real_type,
                                          uint32_t expected_tag,
                                          ASN1_Class class_tag = ASN1_Class::ContextSpecific) {
         BER_Object obj = get_next_object();

         const ASN1_Type type_tag = static_cast<ASN1_Type>(expected_tag);

         if(obj.is_a(type_tag, class_tag)) {
            if(class_tag == ASN1_Class::ExplicitContextSpecific) {
               BER_Decoder(obj, m_limits).decode(out, real_type).verify_end();
            } else {
               push_back(std::move(obj));
               decode(out, real_type, type_tag, class_tag);
            }
         } else {
            out.clear();
            push_back(std::move(obj));
         }

         return (*this);
      }

      template <typename Alloc>
      BER_Decoder& decode_optional_string(std::vector<uint8_t, Alloc>& out,
                                          ASN1_Type real_type,
                                          ASN1_Type expected_tag,
                                          ASN1_Class class_tag = ASN1_Class::ContextSpecific) {
         return decode_optional_string(out, real_type, static_cast<uint32_t>(expected_tag), class_tag);
      }

      ~BER_Decoder();

   private:
      BER_Decoder(BER_Object&& obj, BER_Decoder* parent);

      std::optional<uint8_t> read_next_byte();

      BER_Object get_next_value(size_t sizeofT, ASN1_Type type_tag, ASN1_Class class_tag);

      Limits m_limits;
      BER_Decoder* m_parent = nullptr;
      BER_Object m_pushed;
      // either m_data_src.get() or an unowned pointer
      DataSource* m_source;
      std::unique_ptr<DataSource> m_data_src;
};

/*
* Decode an OPTIONAL or DEFAULT element
*/
template <typename T>
BER_Decoder& BER_Decoder::decode_optional(std::optional<T>& optval, ASN1_Type type_tag, ASN1_Class class_tag) {
   BER_Object obj = get_next_object();

   if(obj.is_a(type_tag, class_tag)) {
      T out{};
      if(class_tag == ASN1_Class::ExplicitContextSpecific) {
         BER_Decoder(obj, m_limits).decode(out).verify_end();
      } else {
         this->push_back(std::move(obj));
         this->decode(out, type_tag, class_tag);
      }
      optval = std::move(out);
   } else {
      this->push_back(std::move(obj));
      optval = std::nullopt;
   }

   return (*this);
}

/*
* Decode an OPTIONAL or DEFAULT element
*/
template <typename T>
BER_Decoder& BER_Decoder::decode_optional_implicit(T& out,
                                                   ASN1_Type type_tag,
                                                   ASN1_Class class_tag,
                                                   ASN1_Type real_type,
                                                   ASN1_Class real_class,
                                                   const T& default_value) {
   BER_Object obj = get_next_object();

   if(obj.is_a(type_tag, class_tag)) {
      obj.set_tagging(real_type, real_class);
      push_back(std::move(obj));
      decode(out, real_type, real_class);
   } else {
      // Not what we wanted, push it back on the stream
      out = default_value;
      push_back(std::move(obj));
   }

   return (*this);
}

/*
* Decode a list of homogeneously typed values
*/
template <typename T>
BER_Decoder& BER_Decoder::decode_list(std::vector<T>& vec, ASN1_Type type_tag, ASN1_Class class_tag) {
   BER_Decoder list = start_cons(type_tag, class_tag);

   while(list.more_items()) {
      T value;
      list.decode(value);
      vec.push_back(std::move(value));
   }

   list.end_cons();

   return (*this);
}

/*
* Decode an optional list of homogeneously typed values
*/
template <typename T>
bool BER_Decoder::decode_optional_list(std::vector<T>& vec, ASN1_Type type_tag, ASN1_Class class_tag) {
   if(peek_next_object().is_a(type_tag, class_tag)) {
      decode_list(vec, type_tag, class_tag);
      return true;
   }

   return false;
}

}  // namespace Botan

namespace Botan {

class RandomNumberGenerator;

/**
 * Arbitrary precision integer
 */
class BOTAN_PUBLIC_API(2, 0) BigInt final {
   public:
      /**
       * Base enumerator for encoding and decoding
       */
      enum Base : uint16_t /* NOLINT(*-use-enum-class) */ {
         Decimal BOTAN_DEPRECATED("All functions using this enum are deprecated") = 10,
         Hexadecimal BOTAN_DEPRECATED("All functions using this enum are deprecated") = 16,
         Binary BOTAN_DEPRECATED("All functions using this enum are deprecated") = 256
      };

      /**
       * Sign symbol definitions for positive and negative numbers
       */
      enum Sign : uint8_t /* NOLINT(*-use-enum-class) */ { Negative = 0, Positive = 1 };

      /**
       * Create empty (zero) BigInt
       */
      BigInt() = default;

      /**
       * Create a 0-value BigInt
       */
      static BigInt zero() { return BigInt(); }

      /**
       * Create a 1-value BigInt
       */
      static BigInt one() { return BigInt::from_u64(1); }

      /**
       * Create BigInt from an unsigned 64 bit integer
       * @param n initial value of this BigInt
       */
      static BigInt from_u64(uint64_t n);

      /**
       * Create BigInt from a word (limb)
       * @param n initial value of this BigInt
       */
      //BOTAN_DEPRECATED("Use BigInt::from_u64 instead")
      static BigInt from_word(word n);

      /**
       * Create BigInt from a signed 32 bit integer
       * @param n initial value of this BigInt
       */
      BOTAN_DEPRECATED("Use BigInt::from_u64 plus negation if required instead") static BigInt from_s32(int32_t n);

      /**
       * Create BigInt from an unsigned 64 bit integer
       * @param n initial value of this BigInt
       *
       * Prefer BigInt::from_u64
       */
      BigInt(uint64_t n);  // NOLINT(*-explicit-conversions) TODO(Botan4) make this explicit

      /**
       * Copy Constructor
       * @param other the BigInt to copy
       */
      BigInt(const BigInt& other) = default;

      /**
       * Create BigInt from a string. If the string starts with 0x the
       * rest of the string will be interpreted as hexadecimal digits.
       * Otherwise, it will be interpreted as a decimal number.
       *
       * @param str the string to parse for an integer value
       */
      //BOTAN_DEPRECATED("Use BigInt::from_string")
      explicit BigInt(std::string_view str) { *this = BigInt::from_string(str); }

      /**
       * Create BigInt from a string.
       *
       * If the string starts with 0x the rest of the string will be
       * interpreted as hexadecimal digits. Otherwise, it will be
       * interpreted as a decimal number.
       *
       * A prefix of "-" will result in a negative integer
       *
       * @param str the string to parse for an integer value
       */
      static BigInt from_string(std::string_view str);

      /**
       * Create BigInt from a sequence of digits
       *
       * The string is interpreted as a sequence of digits in base @p radix.
       *
       * Each character must be interpretable as such a digit; there is no support
       * for whitespace or prefixes (eg '0x' or '-').
       *
       * Currently radix must be 10 or 16.
       *
       * @param digits the sequence of digits
       * @param radix the base
       */
      static BigInt from_radix_digits(std::string_view digits, size_t radix);

      /**
       * Create a BigInt from an integer in a byte array
       * @param buf the byte array holding the value
       * @param length size of buf
       */
      BigInt(const uint8_t buf[], size_t length) { assign_from_bytes(std::span{buf, length}); }

      /**
       * Create a BigInt from an integer in a byte array
       * @param bytes the byte vector holding the value
       */
      explicit BigInt(std::span<const uint8_t> bytes) { assign_from_bytes(bytes); }

      /**
       * Create a BigInt from an integer in a byte array
       * @param buf the byte array holding the value
       * @param length size of buf
       * @param base is the number base of the integer in buf
       */
      BOTAN_DEPRECATED("For hex/decimal use from_string") BigInt(const uint8_t buf[], size_t length, Base base);

      /**
       * Create a BigInt from an integer in a byte array
       *
       * Note this function is primarily used for implementing signature
       * schemes and is not useful in typical applications.
       *
       * @param buf the byte array holding the value
       * @param length size of buf
       * @param max_bits if the resulting integer is more than max_bits,
       *        it will be shifted so it is at most max_bits in length.
       */
      BOTAN_DEPRECATED("Deprecated no replacement")
      static BigInt from_bytes_with_max_bits(const uint8_t buf[], size_t length, size_t max_bits);

      /**
       * @brief Create a random BigInt of the specified size
       *
       * @param rng random number generator
       * @param bits size in bits
       * @param set_high_bit if true, the highest bit is always set
       *
       * @see randomize
       */
      BigInt(RandomNumberGenerator& rng, size_t bits, bool set_high_bit = true);

      /**
       * Create BigInt of specified size, all zeros
       * @param n size of the internal register in words
       */
      BOTAN_DEPRECATED("Deprecated no replacement") static BigInt with_capacity(size_t n);

      /**
       * Move constructor
       */
      BigInt(BigInt&& other) noexcept { this->swap(other); }

      ~BigInt() { _const_time_unpoison(); }

      /**
       * Move assignment
       */
      BigInt& operator=(BigInt&& other) noexcept {
         if(this != &other) {
            this->swap(other);
         }

         return (*this);
      }

      /**
       * Copy assignment
       */
      BigInt& operator=(const BigInt&) = default;

      /**
       * Swap this value with another
       * @param other BigInt to swap values with
       */
      void swap(BigInt& other) noexcept {
         m_data.swap(other.m_data);
         std::swap(m_signedness, other.m_signedness);
      }

      friend void swap(BigInt& x, BigInt& y) noexcept { x.swap(y); }

      BOTAN_DEPRECATED("Deprecated no replacement") void swap_reg(secure_vector<word>& reg) {
         m_data.swap(reg);
         // sign left unchanged
      }

      /**
       * += operator
       * @param y the BigInt to add to this
       */
      BigInt& operator+=(const BigInt& y) { return add(y._data(), y.sig_words(), y.sign()); }

      /**
       * += operator
       * @param y the word to add to this
       */
      BigInt& operator+=(word y) { return add(&y, 1, Positive); }

      /**
       * -= operator
       * @param y the BigInt to subtract from this
       */
      BigInt& operator-=(const BigInt& y) { return sub(y._data(), y.sig_words(), y.sign()); }

      /**
       * -= operator
       * @param y the word to subtract from this
       */
      BigInt& operator-=(word y) { return sub(&y, 1, Positive); }

      /**
       * *= operator
       * @param y the BigInt to multiply with this
       */
      BigInt& operator*=(const BigInt& y);

      /**
       * *= operator
       * @param y the word to multiply with this
       */
      BigInt& operator*=(word y);

      /**
       * /= operator
       * @param y the BigInt to divide this by
       */
      BigInt& operator/=(const BigInt& y);

      /**
       * Modulo operator
       * @param y the modulus to reduce this by
       */
      BigInt& operator%=(const BigInt& y);

      /**
       * Modulo operator
       * @param y the modulus (word) to reduce this by
       */
      word operator%=(word y);

      /**
       * Left shift operator
       * @param shift the number of bits to shift this left by
       */
      BigInt& operator<<=(size_t shift);

      /**
       * Right shift operator
       * @param shift the number of bits to shift this right by
       */
      BigInt& operator>>=(size_t shift);

      /**
       * Increment operator
       */
      BigInt& operator++() { return (*this += 1); }

      /**
       * Decrement operator
       */
      BigInt& operator--() { return (*this -= 1); }

      /**
       * Postfix increment operator
       */
      BigInt operator++(int) {
         BigInt x = (*this);
         ++(*this);
         return x;
      }

      /**
       * Postfix decrement operator
       */
      BigInt operator--(int) {
         BigInt x = (*this);
         --(*this);
         return x;
      }

      /**
       * Unary negation operator
       * @return negative this
       */
      BigInt operator-() const;

      /**
       * ! operator
       * @return true iff this is zero, otherwise false
       */
      bool operator!() const { return is_zero(); }

      //BOTAN_DEPRECATED("Just use operator+/operator-")
      static BigInt add2(const BigInt& x, const word y[], size_t y_words, Sign y_sign);

      //BOTAN_DEPRECATED("Just use operator+/operator-")
      BigInt& add(const word y[], size_t y_words, Sign sign);

      //BOTAN_DEPRECATED("Just use operator+/operator-")
      BigInt& sub(const word y[], size_t y_words, Sign sign) {
         return add(y, y_words, sign == Positive ? Negative : Positive);
      }

      /**
       * Multiply this with y
       * @param y the BigInt to multiply with this
       * @param ws a temp workspace
       */
      BOTAN_DEPRECATED("Just use operator*") BigInt& mul(const BigInt& y, secure_vector<word>& ws);

      /**
       * Square value of *this
       * @param ws a temp workspace
       */
      BOTAN_DEPRECATED("Deprecated no replacement") BigInt& square(secure_vector<word>& ws);

      /**
       * Set *this to y - *this
       * @param y the BigInt to subtract from as a sequence of words
       * @param y_words length of y in words
       * @param ws a temp workspace
       */
      BOTAN_DEPRECATED("Deprecated no replacement")
      BigInt& rev_sub(const word y[], size_t y_words, secure_vector<word>& ws);

      /**
       * Set *this to (*this + y) % mod
       * This function assumes *this is >= 0 && < mod
       * @param y the BigInt to add - assumed y >= 0 and y < mod
       * @param mod the positive modulus
       * @param ws a temp workspace
       */
      BOTAN_DEPRECATED("Deprecated no replacement")
      BigInt& mod_add(const BigInt& y, const BigInt& mod, secure_vector<word>& ws);

      /**
       * Set *this to (*this - y) % mod
       * This function assumes *this is >= 0 && < mod
       * @param y the BigInt to subtract - assumed y >= 0 and y < mod
       * @param mod the positive modulus
       * @param ws a temp workspace
       */
      BOTAN_DEPRECATED("Deprecated no replacement")
      BigInt& mod_sub(const BigInt& y, const BigInt& mod, secure_vector<word>& ws);

      /**
       * Set *this to (*this * y) % mod
       * This function assumes *this is >= 0 && < mod
       * y should be small, less than 16
       * @param y the small integer to multiply by
       * @param mod the positive modulus
       * @param ws a temp workspace
       */
      BOTAN_DEPRECATED("Deprecated no replacement")
      BigInt& mod_mul(uint8_t y, const BigInt& mod, secure_vector<word>& ws);

      /**
       * Return *this % mod
       *
       * Assumes that *this is (if anything) only slightly larger than
       * mod and performs repeated subtractions. It should not be used if
       * *this is much larger than mod, instead use modulo operator.
       */
      BOTAN_DEPRECATED("Deprecated no replacement") size_t reduce_below(const BigInt& mod, secure_vector<word>& ws);

      /**
       * Return *this % mod
       *
       * Assumes that *this is (if anything) only slightly larger than mod and
       * performs repeated subtractions. It should not be used if *this is much
       * larger than mod, instead use modulo operator.
       *
       * Performs exactly bound subtractions, so if *this is >= bound*mod then the
       * result will not be fully reduced. If bound is zero, nothing happens.
       */
      BOTAN_DEPRECATED("Deprecated no replacement")
      void ct_reduce_below(const BigInt& mod, secure_vector<word>& ws, size_t bound);

      /**
       * Zeroize the BigInt. The size of the underlying register is not
       * modified.
       */
      void clear() {
         m_data.set_to_zero();
         m_signedness = Positive;
      }

      /**
       * Compare this to another BigInt
       * @param n the BigInt value to compare with
       * @param check_signs include sign in comparison?
       * @result if (this<n) return -1, if (this>n) return 1, if both
       * values are identical return 0 [like Perl's <=> operator]
       */
      int32_t cmp(const BigInt& n, bool check_signs = true) const;

      /**
       * Compare this to another BigInt
       * @param n the BigInt value to compare with
       * @result true if this == n or false otherwise
       */
      bool is_equal(const BigInt& n) const;

      /**
       * Compare this to another BigInt
       * @param n the BigInt value to compare with
       * @result true if this < n or false otherwise
       */
      bool is_less_than(const BigInt& n) const;

      /**
       * Compare this to an integer
       * @param n the value to compare with
       * @result if (this<n) return -1, if (this>n) return 1, if both
       * values are identical return 0 [like Perl's <=> operator]
       */
      int32_t cmp_word(word n) const;

      /**
       * Test if the integer has an even value
       * @result true if the integer is even, false otherwise
       */
      bool is_even() const { return !get_bit(0); }

      /**
       * Test if the integer has an odd value
       * @result true if the integer is odd, false otherwise
       */
      bool is_odd() const { return get_bit(0); }

      /**
       * Return the signum of this integer
       * @result -1 if negative, 0 if zero, 1 if positive
       */
      int signum() const {
         if(sig_words() == 0) {
            return 0;
         }
         return (sign() == Negative) ? -1 : 1;
      }

      /**
       * Test if the integer is not zero
       * @result true if the integer is non-zero, false otherwise
       */
      BOTAN_DEPRECATED("Use signum() != 0") bool is_nonzero() const { return signum() != 0; }

      /**
       * Test if the integer is zero
       * @result true if the integer is zero, false otherwise
       */
      bool is_zero() const { return sig_words() == 0; }

      /**
       * Set bit at specified position
       * @param n bit position to set
       */
      void set_bit(size_t n) { conditionally_set_bit(n, true); }

      /**
       * Conditionally set bit at specified position. Note if set_it is
       * false, nothing happens, and if the bit is already set, it
       * remains set.
       *
       * @param n bit position to set
       * @param set_it if the bit should be set
       */
      void conditionally_set_bit(size_t n, bool set_it) {
         const size_t which = n / (sizeof(word) * 8);
         const word mask = static_cast<word>(set_it) << (n % (sizeof(word) * 8));
         m_data.set_word_at(which, word_at(which) | mask);
      }

      /**
       * Clear bit at specified position
       * @param n bit position to clear
       */
      void clear_bit(size_t n);

      /**
       * Clear all but the lowest n bits
       * @param n amount of bits to keep
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void mask_bits(size_t n) { m_data.mask_bits(n); }

      /**
       * Return bit value at specified position
       * @param n the bit offset to test
       * @result true, if the bit at position n is set, false otherwise
       */
      bool get_bit(size_t n) const { return ((word_at(n / (sizeof(word) * 8)) >> (n % (sizeof(word) * 8))) & 1) == 1; }

      /**
       * Return (a maximum of) 32 bits of the complete value
       * @param offset the offset to start extracting
       * @param length amount of bits to extract (starting at offset)
       * @result the integer extracted from the register starting at
       * offset with specified length
       */
      BOTAN_DEPRECATED("Deprecated no replacement") uint32_t get_substring(size_t offset, size_t length) const;

      /**
       * Convert this value into a uint32_t, if it is in the range
       * [0 ... 2**32-1], or otherwise throw an exception.
       * @result the value as a uint32_t if conversion is possible
       */
      BOTAN_DEPRECATED("Deprecated no replacement") uint32_t to_u32bit() const;

      /**
       * Convert this value to a decimal string.
       * Warning: decimal conversions are relatively slow
       *
       * If the integer is zero then "0" is returned.
       * If the integer is negative then "-" is prefixed.
       */
      std::string to_dec_string() const;

      /**
       * Convert this value to a hexadecimal string.
       *
       * If the integer is negative then "-" is prefixed.
       * Then a prefix of "0x" is added.
       * Follows is a sequence of hexadecimal characters in uppercase.
       *
       * The number of hexadecimal characters is always an even number,
       * with a zero prefix being included if necessary.
       * For example encoding the integer "5" results in "0x05"
       */
      std::string to_hex_string() const;

      /**
       * @param n the offset to get a byte from
       * @result byte at offset n
       */
      uint8_t byte_at(size_t n) const;

      /**
       * Return the word at a specified position of the internal register
       * @param n position in the register
       * @return value at position n
       */
      word word_at(size_t n) const { return m_data.get_word_at(n); }

      BOTAN_DEPRECATED("Deprecated no replacement") void set_word_at(size_t i, word w) { m_data.set_word_at(i, w); }

      BOTAN_DEPRECATED("Deprecated no replacement") void set_words(const word w[], size_t len) {
         m_data.set_words(w, len);
      }

      /**
       * Tests if the sign of the integer is negative
       * @result true, iff the integer has a negative sign
       */
      BOTAN_DEPRECATED("Use signum() < 0") bool is_negative() const { return signum() < 0; }

      /**
       * Tests if the sign of the integer is positive
       *
       * Note that this is testing the sign, thus it returns true also for zero
       * Prefer signum which is unambiguous
       *
       * @result true, iff the integer has a positive sign
       */
      BOTAN_DEPRECATED("Use signum() >= 0 or signum() > 0 as appropriate") bool is_positive() const {
         return signum() >= 0;
      }

      /**
       * Return the sign of the integer
       * @result the sign of the integer
       */
      Sign sign() const { return (m_signedness); }

      /**
       * @result the opposite sign of the represented integer value
       */
      Sign reverse_sign() const {
         if(sign() == Positive) {
            return Negative;
         }
         return Positive;
      }

      /**
       * Flip the sign of this BigInt
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void flip_sign() { set_sign(reverse_sign()); }

      /**
       * Set sign of the integer
       * @param sign new Sign to set
       */
      void set_sign(Sign sign) {
         if(sign == Negative && is_zero()) {
            sign = Positive;
         }

         m_signedness = sign;
      }

      /**
       * @result absolute (positive) value of this
       */
      BigInt abs() const;

      /**
       * Give size of internal register
       * @result size of internal register in words
       */
      size_t size() const { return m_data.size(); }

      /**
       * Return how many words we need to hold this value
       * @result significant words of the represented integer value
       */
      size_t sig_words() const { return m_data.sig_words(); }

      /**
       * Give byte length of the integer
       * @result byte length of the represented integer value
       */
      size_t bytes() const;

      /**
       * Get the bit length of the integer
       * @result bit length of the represented integer value
       */
      size_t bits() const;

      /**
       * Get the number of high bits unset in the top (allocated) word
       * of this integer. Returns (sizeof(word) * 8) only iff *this is
       * zero. Ignores sign.
       */
      BOTAN_DEPRECATED("Deprecated no replacement") size_t top_bits_free() const;

      /**
       * Return a mutable pointer to the register
       * @result a pointer to the start of the internal register
       */
      BOTAN_DEPRECATED("Deprecated no replacement") word* mutable_data() { return m_data.mutable_data(); }

      /**
       * Return a const pointer to the register
       * @result a pointer to the start of the internal register
       */
      BOTAN_DEPRECATED("Deprecated no replacement") const word* data() const { return m_data.const_data(); }

      /**
       * Don't use this function in application code
       */
      BOTAN_DEPRECATED("Deprecated no replacement") secure_vector<word>& get_word_vector() {
         return m_data.mutable_vector();
      }

      /**
       * Don't use this function in application code
       */
      BOTAN_DEPRECATED("Deprecated no replacement") const secure_vector<word>& get_word_vector() const {
         return m_data.const_vector();
      }

      /**
       * Increase internal register buffer to at least n words
       * @param n new size of register
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void grow_to(size_t n) const { m_data.grow_to(n); }

      BOTAN_DEPRECATED("Deprecated no replacement") void resize(size_t s) { m_data.resize(s); }

      /**
       * Fill BigInt with a random number with size of bitsize
       *
       * If \p set_high_bit is true, the highest bit will be set, which causes
       * the entropy to be \a bits-1. Otherwise the highest bit is randomly chosen
       * by the rng, causing the entropy to be \a bits.
       *
       * @param rng the random number generator to use
       * @param bitsize number of bits the created random value should have
       * @param set_high_bit if true, the highest bit is always set
       */
      void randomize(RandomNumberGenerator& rng, size_t bitsize, bool set_high_bit = true);

      /**
       * Serialize the absolute value of this BigInt as a big endian
       * encoding.
       *
       * If out is smaller than the total bytes of the BigInt then
       * an exception is thrown.
       *
       * If out is larger than the total bytes of the BigInt then the
       * necessary number of zeros are prefixed to produce the desired
       * output length
       *
       * Zero-padding the binary encoding is useful to ensure that other
       * applications correctly parse the encoded value as "positive integer",
       * as a leading 1-bit may be interpreted as a sign bit. It also is
       * necessary when using a fixed size encoding for the integers.
       *
       * @param out destination byte span for the integer value
       */
      void serialize_to(std::span<uint8_t> out) const;

      /**
       * Serialize the value of this BigInt as a big endian encoding,
       * always returning the specified number of bytes.
       *
       * Throws if the BigInt is too large to encode in the length
       * specified.
       */
      template <typename T = std::vector<uint8_t>>
      T serialize(size_t len) const {
         // TODO this supports std::vector and secure_vector
         // it would be nice if this also could work with std::array as in
         //   bn.serialize_to<std::array<uint8_t, 32>>(32);
         T out(len);
         this->serialize_to(out);
         return out;
      }

      /**
       * Serialize the value of this BigInt as a big endian encoding.
       */
      template <typename T = std::vector<uint8_t>>
      T serialize() const {
         return serialize<T>(this->bytes());
      }

      /**
       * Store BigInt-value in a given byte array
       * @param buf destination byte array for the integer value
       */
      BOTAN_DEPRECATED("Use BigInt::serialize_to") void binary_encode(uint8_t buf[]) const {
         this->serialize_to(std::span{buf, this->bytes()});
      }

      /**
       * Store BigInt-value in a given byte array. If len is less than
       * the size of the value, then it will be truncated. If len is
       * greater than the size of the value, it will be zero-padded.
       * If len exactly equals this->bytes(), this function behaves identically
       * to binary_encode.
       *
       * Zero-padding the binary encoding is useful to ensure that other
       * applications correctly parse the encoded value as "positive integer",
       * as a leading 1-bit may be interpreted as a sign bit.
       *
       * @param buf destination byte array for the integer value
       * @param len how many bytes to write
       */
      BOTAN_DEPRECATED("Use BigInt::serialize_to") void binary_encode(uint8_t buf[], size_t len) const;

      /**
       * Read integer value from a byte array with given size
       * @param buf byte array buffer containing the integer
       * @param length size of buf
       */
      BOTAN_DEPRECATED("Use BigInt::from_bytes") void binary_decode(const uint8_t buf[], size_t length) {
         this->assign_from_bytes(std::span{buf, length});
      }

      /**
       * Read integer value from a byte vector
       * @param buf the vector to load from
       */
      BOTAN_DEPRECATED("Use BigInt::from_bytes") void binary_decode(std::span<const uint8_t> buf) {
         this->assign_from_bytes(buf);
      }

      /**
       * Place the value into out, zero-padding up to size words
       * Throw if *this cannot be represented in size words
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void encode_words(word out[], size_t size) const;

      /**
       * If predicate is true assign other to *this
       * Uses a masked operation to avoid side channels
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void ct_cond_assign(bool predicate, const BigInt& other);

      /**
       * If predicate is true swap *this and other
       * Uses a masked operation to avoid side channels
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void ct_cond_swap(bool predicate, BigInt& other);

      /**
       * If predicate is true add value to *this
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void ct_cond_add(bool predicate, const BigInt& value);

      /**
       * Shift @p shift bits to the left, runtime is independent of
       * the value of @p shift.
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void ct_shift_left(size_t shift);

      /**
       * If predicate is true flip the sign of *this
       */
      void cond_flip_sign(bool predicate);

      BOTAN_DEPRECATED("replaced by internal API") void const_time_poison() const { _const_time_poison(); }

      BOTAN_DEPRECATED("replaced by internal API") void const_time_unpoison() const { _const_time_unpoison(); }

      /**
       * @param rng a random number generator
       * @param min the minimum value (must be non-negative)
       * @param max the maximum value (must be non-negative and > min)
       * @return random integer in [min,max)
       */
      static BigInt random_integer(RandomNumberGenerator& rng, const BigInt& min, const BigInt& max);

      /**
       * Create a power of two
       * @param n the power of two to create
       * @return bigint representing 2^n
       */
      static BigInt power_of_2(size_t n) {
         BigInt b;
         b.set_bit(n);
         return b;
      }

      /**
       * Encode the integer value from a BigInt to a std::vector of bytes
       * @param n the BigInt to use as integer source
       * @result secure_vector of bytes containing the bytes of the integer
       */
      BOTAN_DEPRECATED("Use BigInt::serialize") static std::vector<uint8_t> encode(const BigInt& n) {
         return n.serialize<std::vector<uint8_t>>(n.bytes());
      }

      /**
       * Encode the integer value from a BigInt to a secure_vector of bytes
       * @param n the BigInt to use as integer source
       * @result secure_vector of bytes containing the bytes of the integer
       */
      BOTAN_DEPRECATED("Use BigInt::serialize") static secure_vector<uint8_t> encode_locked(const BigInt& n) {
         return n.serialize<secure_vector<uint8_t>>(n.bytes());
      }

      /**
       * Create a BigInt from an integer in a byte array
       * @param bytes the binary value to load
       * @result BigInt representing the integer in the byte array
       */
      static BigInt from_bytes(std::span<const uint8_t> bytes);

      /**
       * Create a BigInt from an integer in a byte array
       * @param buf the binary value to load
       * @param length size of buf
       * @result BigInt representing the integer in the byte array
       */
      BOTAN_DEPRECATED("Use BigInt::from_bytes") static BigInt decode(const uint8_t buf[], size_t length) {
         return BigInt::from_bytes(std::span{buf, length});
      }

      /**
       * Create a BigInt from an integer in a byte array
       * @param buf the binary value to load
       * @result BigInt representing the integer in the byte array
       */
      BOTAN_DEPRECATED("Use BigInt::from_bytes") static BigInt decode(std::span<const uint8_t> buf) {
         return BigInt::from_bytes(buf);
      }

      /**
       * Create a BigInt from an integer in a byte array
       * @param buf the binary value to load
       * @param length size of buf
       * @param base number-base of the integer in buf
       * @result BigInt representing the integer in the byte array
       */
      BOTAN_DEPRECATED("For decimal/hex use from_string")
      static BigInt decode(const uint8_t buf[], size_t length, Base base);

      /**
       * Create a BigInt from an integer in a byte array
       * @param buf the binary value to load
       * @param base number-base of the integer in buf
       * @result BigInt representing the integer in the byte array
       */
      BOTAN_DEPRECATED("For decimal/hex use from_string") static BigInt decode(std::span<const uint8_t> buf, Base base);

      /**
       * Encode a BigInt to a byte array according to IEEE 1363
       * @param n the BigInt to encode
       * @param bytes the length of the resulting secure_vector<uint8_t>
       * @result a secure_vector<uint8_t> containing the encoded BigInt
       */
      BOTAN_DEPRECATED("Use BigInt::serialize")
      static secure_vector<uint8_t> encode_1363(const BigInt& n, size_t bytes) {
         return n.serialize<secure_vector<uint8_t>>(bytes);
      }

      BOTAN_DEPRECATED("Use BigInt::serialize_to") static void encode_1363(std::span<uint8_t> out, const BigInt& n) {
         n.serialize_to(out);
      }

      BOTAN_DEPRECATED("Use BigInt::serialize_to")
      static void encode_1363(uint8_t out[], size_t bytes, const BigInt& n) {
         n.serialize_to(std::span{out, bytes});
      }

      /**
       * Encode two BigInt to a byte array according to IEEE 1363
       * @param n1 the first BigInt to encode
       * @param n2 the second BigInt to encode
       * @param bytes the length of the encoding of each single BigInt
       * @result a secure_vector<uint8_t> containing the concatenation of the two encoded BigInt
       */
      BOTAN_DEPRECATED("Deprecated no replacement")
      static secure_vector<uint8_t> encode_fixed_length_int_pair(const BigInt& n1, const BigInt& n2, size_t bytes);

      /**
       * Return a span over the register
       *
       * @warning this is an implementation detail which is not for
       * public use and not covered by SemVer.
       *
       * @result span over the internal register
       */
      std::span<const word> _as_span() const { return m_data.const_span(); }

      /**
       * Return a const pointer to the register
       *
       * @warning this is an implementation detail which is not for
       * public use and not covered by SemVer.
       *
       * @result a pointer to the start of the internal register
       */
      const word* _data() const { return m_data.const_data(); }

      /**
       * Read integer value from a byte vector (big endian)
       *
       * @warning this is an implementation detail which is not for
       * public use and not covered by SemVer. In applications use
       * BigInt::from_bytes
       *
       * @param bytes the span of bytes to load
       */
      void _assign_from_bytes(std::span<const uint8_t> bytes) { assign_from_bytes(bytes); }

      /**
       * Create a BigInt from a word vector
       *
       * @warning this is an implementation detail which is not for
       * public use and not covered by SemVer.
       */
      static BigInt _from_words(secure_vector<word>& words) {
         BigInt bn;
         bn.m_data.swap(words);
         return bn;
      }

      /**
       * Mark this BigInt as holding secret data
       *
       * @warning this is an implementation detail which is not for
       * public use and not covered by SemVer.
       */
      void _const_time_poison() const;

      /**
       * Mark this BigInt as no longer holding secret data
       *
       * @warning this is an implementation detail which is not for
       * public use and not covered by SemVer.
       */
      void _const_time_unpoison() const;

   private:
      /**
       * Read integer value from a byte vector (big endian)
       * @param bytes the span of bytes to load
       */
      void assign_from_bytes(std::span<const uint8_t> bytes);

      class Data final {
         public:
            word* mutable_data() {
               invalidate_sig_words();
               return m_reg.data();
            }

            const word* const_data() const { return m_reg.data(); }

            std::span<const word> const_span() const { return std::span{m_reg}; }

            secure_vector<word>& mutable_vector() {
               invalidate_sig_words();
               return m_reg;
            }

            const secure_vector<word>& const_vector() const { return m_reg; }

            word get_word_at(size_t n) const {
               if(n < m_reg.size()) {
                  return m_reg[n];
               }
               return 0;
            }

            void set_word_at(size_t i, word w) {
               invalidate_sig_words();
               if(i >= m_reg.size()) {
                  if(w == 0) {
                     return;
                  }
                  grow_to(i + 1);
               }
               m_reg[i] = w;
            }

            void set_words(const word w[], size_t len) {
               invalidate_sig_words();
               m_reg.assign(w, w + len);
            }

            void set_to_zero();

            void mask_bits(size_t n);

            void grow_to(size_t n) const {
               if(n > size()) {
                  if(n <= m_reg.capacity()) {
                     m_reg.resize(n);
                  } else {
                     m_reg.resize(n + (8 - (n % 8)));
                  }
               }
            }

            size_t size() const { return m_reg.size(); }

            void shrink_to_fit(size_t min_size = 0) {
               const size_t words = std::max(min_size, sig_words());
               m_reg.resize(words);
            }

            void resize(size_t s) { m_reg.resize(s); }

            void swap(Data& other) noexcept {
               m_reg.swap(other.m_reg);
               std::swap(m_sig_words, other.m_sig_words);
            }

            void swap(secure_vector<word>& reg) noexcept {
               m_reg.swap(reg);
               invalidate_sig_words();
            }

            void invalidate_sig_words() const noexcept { m_sig_words = sig_words_npos; }

            size_t sig_words() const {
               if(m_sig_words == sig_words_npos) {
                  m_sig_words = calc_sig_words();
               }
               return m_sig_words;
            }

         private:
            static const size_t sig_words_npos = static_cast<size_t>(-1);

            size_t calc_sig_words() const;

            mutable secure_vector<word> m_reg;
            mutable size_t m_sig_words = sig_words_npos;
      };

      Data m_data;
      Sign m_signedness = Positive;
};

/*
* Arithmetic Operators
*/
inline BigInt operator+(const BigInt& x, const BigInt& y) {
   return BigInt::add2(x, y._data(), y.sig_words(), y.sign());
}

inline BigInt operator+(const BigInt& x, word y) {
   return BigInt::add2(x, &y, 1, BigInt::Positive);
}

inline BigInt operator+(word x, const BigInt& y) {
   return y + x;
}

inline BigInt operator-(const BigInt& x, const BigInt& y) {
   return BigInt::add2(x, y._data(), y.sig_words(), y.reverse_sign());
}

inline BigInt operator-(const BigInt& x, word y) {
   return BigInt::add2(x, &y, 1, BigInt::Negative);
}

BOTAN_PUBLIC_API(2, 0) BigInt operator*(const BigInt& x, const BigInt& y);
BOTAN_PUBLIC_API(2, 8) BigInt operator*(const BigInt& x, word y);

inline BigInt operator*(word x, const BigInt& y) {
   return y * x;
}

BOTAN_PUBLIC_API(2, 0) BigInt operator/(const BigInt& x, const BigInt& d);
BOTAN_PUBLIC_API(2, 0) BigInt operator/(const BigInt& x, word m);
BOTAN_PUBLIC_API(2, 0) BigInt operator%(const BigInt& x, const BigInt& m);
BOTAN_PUBLIC_API(2, 0) word operator%(const BigInt& x, word m);
BOTAN_PUBLIC_API(2, 0) BigInt operator<<(const BigInt& x, size_t n);
BOTAN_PUBLIC_API(2, 0) BigInt operator>>(const BigInt& x, size_t n);

/*
 * Comparison Operators
 */
inline bool operator==(const BigInt& a, const BigInt& b) {
   return a.is_equal(b);
}

inline bool operator!=(const BigInt& a, const BigInt& b) {
   return !a.is_equal(b);
}

inline bool operator<=(const BigInt& a, const BigInt& b) {
   return (a.cmp(b) <= 0);
}

inline bool operator>=(const BigInt& a, const BigInt& b) {
   return (a.cmp(b) >= 0);
}

inline bool operator<(const BigInt& a, const BigInt& b) {
   return a.is_less_than(b);
}

inline bool operator>(const BigInt& a, const BigInt& b) {
   return b.is_less_than(a);
}

inline bool operator==(const BigInt& a, word b) {
   return (a.cmp_word(b) == 0);
}

inline bool operator!=(const BigInt& a, word b) {
   return (a.cmp_word(b) != 0);
}

inline bool operator<=(const BigInt& a, word b) {
   return (a.cmp_word(b) <= 0);
}

inline bool operator>=(const BigInt& a, word b) {
   return (a.cmp_word(b) >= 0);
}

inline bool operator<(const BigInt& a, word b) {
   return (a.cmp_word(b) < 0);
}

inline bool operator>(const BigInt& a, word b) {
   return (a.cmp_word(b) > 0);
}

/*
 * I/O Operators
 */
BOTAN_DEPRECATED("Use BigInt::to_{hex,dec}_string")
BOTAN_PUBLIC_API(2, 0) std::ostream& operator<<(std::ostream& stream, const BigInt& n);

BOTAN_DEPRECATED("Use BigInt::from_string")
BOTAN_PUBLIC_API(2, 0) std::istream& operator>>(std::istream& stream, BigInt& n);

}  // namespace Botan

namespace Botan {

class OctetString;

/**
* Represents the length requirements on an algorithm key
*/
class BOTAN_PUBLIC_API(2, 0) Key_Length_Specification final {
   public:
      /**
      * Constructor for fixed length keys
      * @param keylen the supported key length
      */
      explicit Key_Length_Specification(size_t keylen) : m_min_keylen(keylen), m_max_keylen(keylen), m_keylen_mod(1) {}

      /**
      * Constructor for variable length keys
      * @param min_k the smallest supported key length
      * @param max_k the largest supported key length
      * @param k_mod the number of bytes the key must be a multiple of
      */
      Key_Length_Specification(size_t min_k, size_t max_k, size_t k_mod = 1) :
            m_min_keylen(min_k), m_max_keylen(max_k > 0 ? max_k : min_k), m_keylen_mod(k_mod) {}

      /**
      * @param length is a key length in bytes
      * @return true iff this length is a valid length for this algo
      */
      bool valid_keylength(size_t length) const {
         return ((length >= m_min_keylen) && (length <= m_max_keylen) && (length % m_keylen_mod == 0));
      }

      /**
      * @return minimum key length in bytes
      */
      size_t minimum_keylength() const { return m_min_keylen; }

      /**
      * @return maximum key length in bytes
      */
      size_t maximum_keylength() const { return m_max_keylen; }

      /**
      * @return key length multiple in bytes
      */
      size_t keylength_multiple() const { return m_keylen_mod; }

      /*
      * Multiplies all length requirements with the given factor
      * @param n the multiplication factor
      * @return a key length specification multiplied by the factor
      */
      Key_Length_Specification multiple(size_t n) const {
         return Key_Length_Specification(n * m_min_keylen, n * m_max_keylen, n * m_keylen_mod);
      }

   private:
      size_t m_min_keylen, m_max_keylen, m_keylen_mod;
};

/**
* This class represents a symmetric algorithm object.
*/
class BOTAN_PUBLIC_API(2, 0) SymmetricAlgorithm {
   public:
      SymmetricAlgorithm() = default;
      virtual ~SymmetricAlgorithm() = default;
      SymmetricAlgorithm(const SymmetricAlgorithm& other) = default;
      SymmetricAlgorithm(SymmetricAlgorithm&& other) = default;
      SymmetricAlgorithm& operator=(const SymmetricAlgorithm& other) = default;
      SymmetricAlgorithm& operator=(SymmetricAlgorithm&& other) = default;

      /**
      * Reset the internal state. This includes not just the key, but
      * any partial message that may have been in process.
      */
      virtual void clear() = 0;

      /**
      * @return object describing limits on key size
      */
      virtual Key_Length_Specification key_spec() const = 0;

      /**
      * @return maximum allowed key length
      */
      size_t maximum_keylength() const { return key_spec().maximum_keylength(); }

      /**
      * @return minimum allowed key length
      */
      size_t minimum_keylength() const { return key_spec().minimum_keylength(); }

      /**
      * Check whether a given key length is valid for this algorithm.
      * @param length the key length to be checked.
      * @return true if the key length is valid.
      */
      bool valid_keylength(size_t length) const { return key_spec().valid_keylength(length); }

      /**
      * Set the symmetric key of this object.
      * @param key the SymmetricKey to be set.
      */
      void set_key(const OctetString& key);

      /**
      * Set the symmetric key of this object.
      * @param key the contiguous byte range to be set.
      */
      void set_key(std::span<const uint8_t> key);

      /**
      * Set the symmetric key of this object.
      * @param key the to be set as a byte array.
      * @param length in bytes of key param
      */
      void set_key(const uint8_t key[], size_t length) { set_key(std::span{key, length}); }

      /**
      * @return the algorithm name
      */
      virtual std::string name() const = 0;

      /**
      * @return true if a key has been set on this object
      */
      virtual bool has_keying_material() const = 0;

   protected:
      void assert_key_material_set() const { assert_key_material_set(has_keying_material()); }

      void assert_key_material_set(bool predicate) const {
         if(!predicate) {
            throw_key_not_set_error();
         }
      }

   private:
      void throw_key_not_set_error() const;

      /**
      * Run the key schedule
      * @param key the key
      */
      virtual void key_schedule(std::span<const uint8_t> key) = 0;
};

}  // namespace Botan

namespace Botan {

/**
* This class represents a block cipher object.
*/
class BOTAN_PUBLIC_API(2, 0) BlockCipher : public SymmetricAlgorithm {
   public:
      /**
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to choose
      * @return a null pointer if the algo/provider combination cannot be found
      */
      static std::unique_ptr<BlockCipher> create(std::string_view algo_spec, std::string_view provider = "");

      /**
      * Create an instance based on a name, or throw if the
      * algo/provider combination cannot be found. If provider is
      * empty then best available is chosen.
      */
      static std::unique_ptr<BlockCipher> create_or_throw(std::string_view algo_spec, std::string_view provider = "");

      /**
      * @return list of available providers for this algorithm, empty if not available
      * @param algo_spec algorithm name
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      /**
      * Multiplier on a block cipher's native parallelism
      *
      * Usually notable performance gains come from further loop blocking,
      * at least for 2 or 4x
      */
      static constexpr size_t ParallelismMult = 4;

      /**
      * @return block size of this algorithm
      */
      virtual size_t block_size() const = 0;

      /**
      * @return native parallelism of this cipher in blocks
      */
      virtual size_t parallelism() const { return 1; }

      /**
      * @return preferred parallelism of this cipher in bytes
      */
      size_t parallel_bytes() const { return parallelism() * block_size() * BlockCipher::ParallelismMult; }

      /**
      * @return provider information about this implementation. Default is "base",
      * might also return "sse2", "avx2", "openssl", or some other arbitrary string.
      */
      virtual std::string provider() const { return "base"; }

      /**
      * Encrypt a block.
      * @param in The plaintext block to be encrypted as a byte array.
      * Must be of length block_size().
      * @param out The byte array designated to hold the encrypted block.
      * Must be of length block_size().
      */
      void encrypt(const uint8_t in[], uint8_t out[]) const { encrypt_n(in, out, 1); }

      /**
      * Decrypt a block.
      * @param in The ciphertext block to be decrypted as a byte array.
      * Must be of length block_size().
      * @param out The byte array designated to hold the decrypted block.
      * Must be of length block_size().
      */
      void decrypt(const uint8_t in[], uint8_t out[]) const { decrypt_n(in, out, 1); }

      /**
      * Encrypt a block.
      * @param block the plaintext block to be encrypted
      * Must be of length block_size(). Will hold the result when the function
      * has finished.
      */
      void encrypt(uint8_t block[]) const { encrypt_n(block, block, 1); }

      /**
      * Decrypt a block.
      * @param block the ciphertext block to be decrypted
      * Must be of length block_size(). Will hold the result when the function
      * has finished.
      */
      void decrypt(uint8_t block[]) const { decrypt_n(block, block, 1); }

      /**
      * Encrypt one or more blocks
      * @param block the input/output buffer (multiple of block_size())
      */
      void encrypt(std::span<uint8_t> block) const {
         return encrypt_n(block.data(), block.data(), block.size() / block_size());
      }

      /**
      * Decrypt one or more blocks
      * @param block the input/output buffer (multiple of block_size())
      */
      void decrypt(std::span<uint8_t> block) const {
         return decrypt_n(block.data(), block.data(), block.size() / block_size());
      }

      /**
      * Encrypt one or more blocks
      * @param in the input buffer (multiple of block_size())
      * @param out the output buffer (same size as in)
      */
      void encrypt(std::span<const uint8_t> in, std::span<uint8_t> out) const {
         return encrypt_n(in.data(), out.data(), in.size() / block_size());
      }

      /**
      * Decrypt one or more blocks
      * @param in the input buffer (multiple of block_size())
      * @param out the output buffer (same size as in)
      */
      void decrypt(std::span<const uint8_t> in, std::span<uint8_t> out) const {
         return decrypt_n(in.data(), out.data(), in.size() / block_size());
      }

      /**
      * Encrypt one or more blocks
      * @param in the input buffer (multiple of block_size())
      * @param out the output buffer (same size as in)
      * @param blocks the number of blocks to process
      */
      virtual void encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const = 0;

      /**
      * Decrypt one or more blocks
      * @param in the input buffer (multiple of block_size())
      * @param out the output buffer (same size as in)
      * @param blocks the number of blocks to process
      */
      virtual void decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const = 0;

      BOTAN_DEPRECATED("Deprecated no replacement")
      void encrypt_n_xex(uint8_t data[], const uint8_t mask[], size_t blocks) const {
         const size_t BS = block_size();
         for(size_t i = 0; i != blocks * BS; ++i) {
            data[i] ^= mask[i];
         }
         encrypt_n(data, data, blocks);
         for(size_t i = 0; i != blocks * BS; ++i) {
            data[i] ^= mask[i];
         }
      }

      BOTAN_DEPRECATED("Deprecated no replacement")
      void decrypt_n_xex(uint8_t data[], const uint8_t mask[], size_t blocks) const {
         const size_t BS = block_size();
         for(size_t i = 0; i != blocks * BS; ++i) {
            data[i] ^= mask[i];
         }
         decrypt_n(data, data, blocks);
         for(size_t i = 0; i != blocks * BS; ++i) {
            data[i] ^= mask[i];
         }
      }

      /**
      * @return new object representing the same algorithm as *this
      */
      virtual std::unique_ptr<BlockCipher> new_object() const = 0;

      BlockCipher* clone() const { return this->new_object().release(); }
};

/**
* Tweakable block ciphers allow setting a tweak which is a non-keyed
* value which affects the encryption/decryption operation.
*/
class BOTAN_PUBLIC_API(2, 8) Tweakable_Block_Cipher : public BlockCipher {
   public:
      /**
      * Set the tweak value. This must be called after setting a key. The value
      * persists until either set_tweak, set_key, or clear is called.
      * Different algorithms support different tweak length(s). If called with
      * an unsupported length, Invalid_Argument will be thrown.
      */
      virtual void set_tweak(const uint8_t tweak[], size_t len) = 0;
};

/**
* Represents a block cipher with a single fixed block size
*/
template <size_t BS, size_t KMIN, size_t KMAX = 0, size_t KMOD = 1, typename BaseClass = BlockCipher>
class Block_Cipher_Fixed_Params : public BaseClass {
   public:
      enum { BLOCK_SIZE = BS }; /* NOLINT(*-enum-size,*-use-enum-class) */

      size_t block_size() const final { return BS; }

      Key_Length_Specification key_spec() const final { return Key_Length_Specification(KMIN, KMAX, KMOD); }
};

}  // namespace Botan

namespace Botan {

/**
* This class represents any kind of computation which uses an internal
* state, such as hash functions or MACs
*/
class BOTAN_PUBLIC_API(2, 0) Buffered_Computation /* NOLINT(*special-member-functions) */ {
   public:
      /**
      * @return length of the output of this function in bytes
      */
      virtual size_t output_length() const = 0;

      /**
      * Add new input to process.
      * @param in the input to process as a byte array
      * @param length of param in in bytes
      */
      void update(const uint8_t in[], size_t length) { add_data({in, length}); }

      /**
      * Add new input to process.
      * @param in the input to process as a contiguous data range
      */
      void update(std::span<const uint8_t> in) { add_data(in); }

      void update_be(uint16_t val);
      void update_be(uint32_t val);
      void update_be(uint64_t val);

      void update_le(uint16_t val);
      void update_le(uint32_t val);
      void update_le(uint64_t val);

      /**
      * Add new input to process.
      * @param str the input to process as a std::string_view. Will be interpreted
      * as a byte array based on the strings encoding.
      */
      void update(std::string_view str);

      /**
      * Process a single byte.
      * @param in the byte to process
      */
      void update(uint8_t in) { add_data({&in, 1}); }

      /**
      * Complete the computation and retrieve the
      * final result.
      * @param out The byte array to be filled with the result.
      * Must be of length output_length()
      */
      void final(uint8_t out[]) { final_result({out, output_length()}); }

      /**
      * Complete the computation and retrieve the
      * final result as a container of your choice.
      * @return a contiguous container holding the result
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T final() {
         T output(output_length());
         final_result(output);
         return output;
      }

      std::vector<uint8_t> final_stdvec() { return final<std::vector<uint8_t>>(); }

      void final(std::span<uint8_t> out);

      template <concepts::resizable_byte_buffer T>
      void final(T& out) {
         out.resize(output_length());
         final_result(out);
      }

      /**
      * Update and finalize computation. Does the same as calling update()
      * and final() consecutively.
      * @param in the input to process as a byte array
      * @param length the length of the byte array
      * @result the result of the call to final()
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T process(const uint8_t in[], size_t length) {
         update(in, length);
         return final<T>();
      }

      /**
      * Update and finalize computation. Does the same as calling update()
      * and final() consecutively.
      * @param in the input to process as a string
      * @result the result of the call to final()
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T process(std::string_view in) {
         update(in);
         return final<T>();
      }

      /**
      * Update and finalize computation. Does the same as calling update()
      * and final() consecutively.
      * @param in the input to process as a contiguous container
      * @result the result of the call to final()
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T process(std::span<const uint8_t> in) {
         update(in);
         return final<T>();
      }

      virtual ~Buffered_Computation() = default;

   private:
      /**
      * Add more data to the computation
      * @param input is an input buffer
      */
      virtual void add_data(std::span<const uint8_t> input) = 0;

      /**
      * Write the final output to out
      * @param out is an output buffer of output_length()
      */
      virtual void final_result(std::span<uint8_t> out) = 0;
};

}  // namespace Botan


namespace Botan {

/**
* IPv4 Address
*/
class BOTAN_PUBLIC_API(3, 12) IPv4Address final {
   public:
      explicit IPv4Address(uint32_t ip) : m_ip(ip) {}

      static std::optional<IPv4Address> from_string(std::string_view str);

      /**
      * Return an address with the leading @p bits set to one and the remainder
      * zero. Throws Invalid_Argument if @p bits > 32.
      */
      static IPv4Address netmask(size_t bits);

      static IPv4Address host_mask() { return netmask(32); }

      IPv4Address operator&(const IPv4Address& other) const { return IPv4Address(m_ip & other.m_ip); }

      auto operator<=>(const IPv4Address&) const = default;

      /// The address as a 32-bit big-endian integer
      uint32_t value() const { return m_ip; }

      /// The address as four bytes, network-byte-order.
      std::array<uint8_t, 4> to_bytes() const;

      /// Dotted-decimal form, e.g. "10.0.0.1".
      std::string to_string() const;

      /**
      * If this value is a netmask consisting of a run of one bits followed by
      * a run of zero bits, return the number of one bits.
      *
      * Otherwise return nullopt.
      */
      std::optional<size_t> prefix_length() const;

   private:
      uint32_t m_ip;
};

/**
* An IPv4 subnet in CIDR form: a network address paired with a prefix length
*/
class BOTAN_PUBLIC_API(3, 12) IPv4Subnet final {
   public:
      /**
      * Construct from a network address and a prefix length in [0, 32].
      * Host bits of @p address are cleared.
      *
      * Throws Invalid_Argument if @p prefix_length > 32.
      */
      IPv4Subnet(IPv4Address address, size_t prefix_length);

      /**
      * Construct from a network address and a netmask (4 bytes each)
      * Returns nullopt if netmask is not a valid contiguous CIDR prefix.
      */
      static std::optional<IPv4Subnet> from_address_and_mask(std::span<const uint8_t, 8> addr_and_mask);

      /**
      * Construct from a network address and a netmask (4 bytes each)
      * Returns nullopt if netmask is not a valid contiguous CIDR prefix.
      */
      static std::optional<IPv4Subnet> from_address_and_mask(uint32_t addr, uint32_t mask);

      /**
      * Parse CIDR-style form "10.0.0.0/8".
      *
      * The "/N" suffix is required: bare addresses should be parsed via
      * IPv4Address::from_string and wrapped with IPv4Subnet::host if needed.
      *
      * Returns nullopt on parse failure or out-of-range prefix length.
      */
      static std::optional<IPv4Subnet> from_string(std::string_view str);

      /**
      * A single-host subnet (prefix length 32) covering exactly @p address.
      */
      static IPv4Subnet host(IPv4Address address) { return IPv4Subnet(address, 32); }

      /// The network address (host bits already zeroed).
      const IPv4Address& address() const { return m_address; }

      /// Prefix length in [0, 32].
      size_t prefix_length() const { return m_prefix_length; }

      /// True iff prefix_length() == 32.
      bool is_host() const { return m_prefix_length == 32; }

      /// True iff @p ip falls within this subnet.
      bool contains(const IPv4Address& ip) const;

      /// CIDR-style "10.0.0.0/8".
      std::string to_string() const;

      /**
      * Bytes for use in a DER-encoded GeneralName iPAddress field:
      *  - 4 bytes (the address) if is_host() — SAN form per RFC 5280 4.2.1.6.
      *  - 8 bytes (address || netmask) otherwise — name constraint form per RFC 5280 4.2.1.10.
      */
      std::vector<uint8_t> serialize() const;

      friend bool operator==(const IPv4Subnet&, const IPv4Subnet&) = default;

   private:
      IPv4Address m_address;
      uint8_t m_prefix_length;
};

}  // namespace Botan

namespace Botan {

class IPv4Address;

/**
* IPv6 Address
*/
class BOTAN_PUBLIC_API(3, 12) IPv6Address final {
   public:
      explicit IPv6Address(std::span<const uint8_t, 16> ip);

      explicit IPv6Address(std::array<uint8_t, 16> ip) : m_ip(ip) {}

      static std::optional<IPv6Address> from_string(std::string_view str);

      /**
      * Return an address with the leading @p bits set to one and the remainder
      * zero. Throws Invalid_Argument if @p bits > 128.
      */
      static IPv6Address netmask(size_t bits);

      static IPv6Address host_mask() { return netmask(128); }

      IPv6Address operator&(const IPv6Address& other) const;

      auto operator<=>(const IPv6Address&) const = default;

      std::span<const uint8_t, 16> address() const { return m_ip; }

      std::string to_string() const;

      /**
      * If this value is a netmask consisting of a run of one bits followed by
      * a run of zero bits, return the number of one bits.
      *
      * Otherwise return nullopt.
      */
      std::optional<size_t> prefix_length() const;

      /**
      * If this IPv6 address is an IPv4-compatible IPv6 address (RFC 4291 2.5.5.1)
      * or an IPv4-mapped IPv6 address (RFC 4291 2.5.5.2), return the embedded
      * IPv4 address.
      */
      std::optional<IPv4Address> as_ipv4() const;

   private:
      std::array<uint8_t, 16> m_ip;
};

/**
* An IPv6 subnet in CIDR form: a network address paired with a prefix length
*/
class BOTAN_PUBLIC_API(3, 12) IPv6Subnet final {
   public:
      /**
      * Construct from a network address and a prefix length in [0, 128].
      * Host bits of @p address are cleared.
      *
      * Throws Invalid_Argument if @p prefix_length > 128.
      */
      IPv6Subnet(IPv6Address address, size_t prefix_length);

      /**
      * Construct from a network address and a 16-byte CIDR netmask.
      * Returns nullopt if netmask is not a valid contiguous CIDR prefix.
      */
      static std::optional<IPv6Subnet> from_address_and_mask(std::span<const uint8_t, 32> addr_and_mask);

      /**
      * Parse the CIDR-style form "2001:db8::/32".
      *
      * The "/N" suffix is required: bare addresses should be parsed via
      * IPv6Address::from_string and wrapped with IPv6Subnet::host if needed.
      *
      * Returns nullopt on parse failure or out-of-range prefix length.
      */
      static std::optional<IPv6Subnet> from_string(std::string_view str);

      /**
      * A single-host subnet (prefix length 128) covering exactly @p address.
      */
      static IPv6Subnet host(IPv6Address address) { return IPv6Subnet(address, 128); }

      /// The network address (host bits already zeroed).
      const IPv6Address& address() const { return m_address; }

      /// Prefix length in [0, 128].
      size_t prefix_length() const { return m_prefix_length; }

      /// True iff prefix_length() == 128.
      bool is_host() const { return m_prefix_length == 128; }

      /// True iff @p ip falls within this subnet.
      bool contains(const IPv6Address& ip) const;

      /// CIDR-style "2001:db8::/32".
      std::string to_string() const;

      /**
      * Bytes for use in a DER-encoded GeneralName iPAddress field:
      *  - 16 bytes (the address) if is_host(); the SAN form per RFC 5280 4.2.1.6.
      *  - 32 bytes (address || netmask) otherwise; the name constraint form
      *    per RFC 5280 4.2.1.10.
      */
      std::vector<uint8_t> serialize() const;

      friend bool operator==(const IPv6Subnet&, const IPv6Subnet&) = default;

   private:
      IPv6Address m_address;
      uint8_t m_prefix_length;
};

}  // namespace Botan

namespace Botan {

class Public_Key;

/**
* Certificate validation status code
*/
enum class Certificate_Status_Code : uint16_t {
   // TODO(Botan4) renumber this, e.g. Validation Errors -> IP_ADDR_BLOCKS_ERROR
   // TODO(Botan4) rename variants to CamelCase
   OK = 0,
   VERIFIED = 0,

   // Revocation status
   OCSP_RESPONSE_GOOD = 1,
   OCSP_SIGNATURE_OK = 2,
   VALID_CRL_CHECKED = 3,
   OCSP_NO_HTTP = 4,

   // Warnings
   FIRST_WARNING_STATUS = 500,
   CERT_SERIAL_NEGATIVE = 500,
   DN_TOO_LONG = 501,
   OCSP_NO_REVOCATION_URL = 502,
   OCSP_SERVER_NOT_AVAILABLE = 503,
   TRUSTED_CERT_HAS_EXPIRED = 504,
   TRUSTED_CERT_NOT_YET_VALID = 505,

   // Errors
   FIRST_ERROR_STATUS = 1000,

   SIGNATURE_METHOD_TOO_WEAK = 1000,
   UNTRUSTED_HASH = 1001,
   NO_REVOCATION_DATA = 1002,
   NO_MATCHING_CRLDP = 1003,
   OCSP_ISSUER_NOT_TRUSTED = 1004,

   // Time problems
   CERT_NOT_YET_VALID = 2000,
   CERT_HAS_EXPIRED = 2001,
   OCSP_NOT_YET_VALID = 2002,
   OCSP_HAS_EXPIRED = 2003,
   CRL_NOT_YET_VALID = 2004,
   CRL_HAS_EXPIRED = 2005,
   OCSP_IS_TOO_OLD = 2006,

   // Revocation checks are skipped for chains which have an error more
   // serious than this because they are anyway invalid
   FIRST_ERROR_STATUS_TO_SKIP_REVOCATION = 3000,

   // Chain generation problems
   CERT_ISSUER_NOT_FOUND = 3000,
   CANNOT_ESTABLISH_TRUST = 3001,
   CERT_CHAIN_LOOP = 3002,
   CHAIN_LACKS_TRUST_ROOT = 3003,
   CHAIN_NAME_MISMATCH = 3004,

   // Validation errors
   POLICY_ERROR = 4000,
   INVALID_USAGE = 4001,
   CERT_CHAIN_TOO_LONG = 4002,
   CA_CERT_NOT_FOR_CERT_ISSUER = 4003,
   NAME_CONSTRAINT_ERROR = 4004,
   IPADDR_BLOCKS_ERROR = 4011,
   AS_BLOCKS_ERROR = 4012,

   // Revocation errors
   CA_CERT_NOT_FOR_CRL_ISSUER = 4005,
   OCSP_CERT_NOT_LISTED = 4006,
   OCSP_BAD_STATUS = 4007,

   // Other problems
   CERT_NAME_NOMATCH = 4008,
   UNKNOWN_CRITICAL_EXTENSION = 4009,
   DUPLICATE_CERT_EXTENSION = 4010,
   OCSP_SIGNATURE_ERROR = 4501,
   OCSP_ISSUER_NOT_FOUND = 4502,
   OCSP_RESPONSE_MISSING_KEYUSAGE = 4503,
   OCSP_RESPONSE_INVALID = 4504,
   EXT_IN_V1_V2_CERT = 4505,
   DUPLICATE_CERT_POLICY = 4506,
   V2_IDENTIFIERS_IN_V1_CERT = 4507,
   EXTENSION_ENCODING_ERROR = 4508,

   // Hard failures
   CERT_IS_REVOKED = 5000,
   CRL_BAD_SIGNATURE = 5001,
   SIGNATURE_ERROR = 5002,
   CERT_PUBKEY_INVALID = 5003,
   SIGNATURE_ALGO_UNKNOWN = 5004,
   SIGNATURE_ALGO_BAD_PARAMS = 5005,
   EXCEEDED_SEARCH_LIMITS = 5006,
};

/**
* Convert a status code to a human readable diagnostic message
* @param code the certificate status
* @return string literal constant, or nullptr if code unknown
*/
BOTAN_PUBLIC_API(2, 0) const char* to_string(Certificate_Status_Code code);

/**
* X.509v3 Key Constraints.
* If updating update copy in ffi.h
*/
class BOTAN_PUBLIC_API(3, 0) Key_Constraints final {
   public:
      enum Bits : uint16_t /* NOLINT(*-use-enum-class) */ {
         None = 0,
         DigitalSignature = 1 << 15,
         NonRepudiation = 1 << 14,
         KeyEncipherment = 1 << 13,
         DataEncipherment = 1 << 12,
         KeyAgreement = 1 << 11,
         KeyCertSign = 1 << 10,
         CrlSign = 1 << 9,
         EncipherOnly = 1 << 8,
         DecipherOnly = 1 << 7,

         // Deprecated SHOUTING_CASE names for Key_Constraints
         // will be removed in a future major release
         NO_CONSTRAINTS BOTAN_DEPRECATED("Use None") = None,
         DIGITAL_SIGNATURE BOTAN_DEPRECATED("Use DigitalSignature") = DigitalSignature,
         NON_REPUDIATION BOTAN_DEPRECATED("Use NonRepudiation") = NonRepudiation,
         KEY_ENCIPHERMENT BOTAN_DEPRECATED("Use KeyEncipherment") = KeyEncipherment,
         DATA_ENCIPHERMENT BOTAN_DEPRECATED("Use DataEncipherment") = DataEncipherment,
         KEY_AGREEMENT BOTAN_DEPRECATED("Use KeyAgreement") = KeyAgreement,
         KEY_CERT_SIGN BOTAN_DEPRECATED("Use KeyCertSign") = KeyCertSign,
         CRL_SIGN BOTAN_DEPRECATED("Use CrlSign") = CrlSign,
         ENCIPHER_ONLY BOTAN_DEPRECATED("Use EncipherOnly") = EncipherOnly,
         DECIPHER_ONLY BOTAN_DEPRECATED("Use DecipherOnly") = DecipherOnly,
      };

      Key_Constraints(const Key_Constraints& other) = default;
      Key_Constraints(Key_Constraints&& other) = default;
      Key_Constraints& operator=(const Key_Constraints& other) = default;
      Key_Constraints& operator=(Key_Constraints&& other) = default;
      ~Key_Constraints() = default;

      // NOLINTNEXTLINE(*-explicit-conversions)
      Key_Constraints(Key_Constraints::Bits bits) : m_value(bits) {}

      explicit Key_Constraints(uint32_t bits) : m_value(bits) {}

      Key_Constraints() : m_value(0) {}

      /**
      * Return typical constraints for a CA certificate.
      *
      * The reasons for KeyCertSign and CrlSign should be obvious
      *
      * CAB baseline requirements are that DigitalSignature should be set
      * if the certificate is used to sign OCSP responses.
      */
      static Key_Constraints ca_constraints() {
         return Key_Constraints(Key_Constraints::KeyCertSign | Key_Constraints::CrlSign |
                                Key_Constraints::DigitalSignature);
      }

      bool operator==(const Key_Constraints&) const = default;

      void operator|=(Key_Constraints::Bits other) { m_value |= other; }

      // Return true if all bits in mask are set
      bool includes(Key_Constraints::Bits other) const { return (m_value & other) == other; }

      bool includes(Key_Constraints other) const { return (m_value & other.m_value) == other.m_value; }

      // Return true if any of the bits provided are set
      bool includes_any(auto&&... bits) const { return (m_value & (bits | ...)) > 0; }

      bool empty() const { return m_value == 0; }

      uint32_t value() const { return m_value; }

      std::string to_string() const;

      /**
      * Check that key constraints are permitted for a specific public key.
      * @param key the public key on which the constraints shall be enforced on
      * @return false if the constraints are not permitted for this key
      */
      bool compatible_with(const Public_Key& key) const;

   private:
      uint32_t m_value;
};

/**
* X.509v2 CRL Reason Code.
*/
enum class CRL_Code : uint8_t {
   Unspecified = 0,
   KeyCompromise = 1,
   CaCompromise = 2,
   AffiliationChanged = 3,
   Superseded = 4,
   CessationOfOperation = 5,
   CertificateHold = 6,
   RemoveFromCrl = 8,
   PrivilegeWithdrawn = 9,
   AaCompromise = 10,
};

enum class Usage_Type : uint8_t {
   UNSPECIFIED,  // no restrictions
   TLS_SERVER_AUTH,
   TLS_CLIENT_AUTH,
   CERTIFICATE_AUTHORITY,
   OCSP_RESPONDER,
   ENCRYPTION
};

}  // namespace Botan

namespace Botan {

class X509_Certificate;
class Public_Key;

BOTAN_DEPRECATED("Use Key_Constraints::to_string")

inline std::string key_constraints_to_string(Key_Constraints c) {
   return c.to_string();
}

/**
* Distinguished Name
*/
class BOTAN_PUBLIC_API(2, 0) X509_DN final : public ASN1_Object {
   public:
      X509_DN() = default;

      X509_DN(std::initializer_list<std::pair<std::string_view, std::string_view>> args) {
         for(const auto& i : args) {
            add_attribute(i.first, i.second);
         }
      }

      /**
      * Since DN matching for Name Constraints requires preserving order and
      * multimaps have sorted keys, this constructor is deprecated.
      */
      BOTAN_DEPRECATED("Deprecated use initializer list constructor")
      explicit X509_DN(const std::multimap<OID, std::string>& args) {
         for(const auto& i : args) {
            add_attribute(i.first, i.second);
         }
      }

      /**
      * Since DN matching for Name Constraints requires preserving order and
      * multimaps have sorted keys, this constructor is deprecated.
      */
      BOTAN_DEPRECATED("Deprecated use initializer list constructor")
      explicit X509_DN(const std::multimap<std::string, std::string>& args) {
         for(const auto& i : args) {
            add_attribute(i.first, i.second);
         }
      }

      void encode_into(DER_Encoder& to) const override;
      void decode_from(BER_Decoder& from) override;

      bool has_field(const OID& oid) const;
      ASN1_String get_first_attribute(const OID& oid) const;

      /*
      * Return the BER encoded data, if any
      */
      const std::vector<uint8_t>& get_bits() const { return m_dn_bits; }

      std::vector<uint8_t> DER_encode() const;

      bool empty() const { return m_rdn.empty(); }

      size_t count() const { return m_rdn.size(); }

      std::string to_string() const;

      /**
      * Return the DN components as a vector. Note that the order of the components is
      * preserved only when using the initializer list constructor.
      */
      const std::vector<std::pair<OID, ASN1_String>>& dn_info() const { return m_rdn; }

      std::multimap<OID, std::string> get_attributes() const;
      std::multimap<std::string, std::string> contents() const;

      bool has_field(std::string_view attr) const;
      std::vector<std::string> get_attribute(std::string_view attr) const;
      std::string get_first_attribute(std::string_view attr) const;

      void add_attribute(std::string_view key, std::string_view val);

      void add_attribute(const OID& oid, std::string_view val) { add_attribute(oid, ASN1_String(val)); }

      void add_attribute(const OID& oid, const ASN1_String& val);

      static std::string deref_info_field(std::string_view key);

      /**
      * Lookup upper bounds in characters for the length of distinguished name fields
      * as given in RFC 5280, Appendix A.
      *
      * @param oid the oid of the DN to lookup
      * @return the upper bound, or zero if no ub is known to Botan
      */
      static size_t lookup_ub(const OID& oid);

   private:
      std::vector<std::pair<OID, ASN1_String>> m_rdn;
      std::vector<uint8_t> m_dn_bits;
};

BOTAN_PUBLIC_API(2, 0) bool operator==(const X509_DN& dn1, const X509_DN& dn2);
BOTAN_PUBLIC_API(2, 0) bool operator!=(const X509_DN& dn1, const X509_DN& dn2);

/*
The ordering here is arbitrary and may change from release to release.
It is intended for allowing DNs as keys in std::map and similar containers
*/
BOTAN_PUBLIC_API(2, 0) bool operator<(const X509_DN& dn1, const X509_DN& dn2);

BOTAN_PUBLIC_API(2, 0) std::ostream& operator<<(std::ostream& out, const X509_DN& dn);
BOTAN_PUBLIC_API(2, 0) std::istream& operator>>(std::istream& in, X509_DN& dn);

/**
* Alternative Name
*/
class BOTAN_PUBLIC_API(2, 0) AlternativeName final : public ASN1_Object {
   public:
      void encode_into(DER_Encoder& to) const override;
      void decode_from(BER_Decoder& from) override;

      /// Create an empty name
      AlternativeName() = default;

      /// Add a URI to this AlternativeName
      void add_uri(std::string_view uri);

      /// Add a URI to this AlternativeName
      void add_email(std::string_view addr);

      /// Add a DNS name to this AlternativeName
      void add_dns(std::string_view dns);

      /// Add an "OtherName" identified by object identifier to this AlternativeName
      void add_other_name(const OID& oid, const ASN1_String& value);

      /// Add a directory name to this AlternativeName
      void add_dn(const X509_DN& dn);

      /// Add an IP address to this alternative name
      void add_ipv4_address(uint32_t ipv4);

      /// Add an IP address to this alternative name
      void add_ipv4_address(IPv4Address ipv4) { add_ipv4_address(ipv4.value()); }

      /// Add an IPv6 address to this alternative name
      void add_ipv6_address(const IPv6Address& ipv6);

      /// Return the set of URIs included in this alternative name
      const std::set<std::string>& uris() const { return m_uri; }

      /// Return the set of email addresses included in this alternative name
      const std::set<std::string>& email() const { return m_email; }

      /// Return the set of DNS names included in this alternative name
      const std::set<std::string>& dns() const { return m_dns; }

      /// Return the set of IPv4 addresses included in this alternative name
      const std::set<uint32_t>& ipv4_address() const { return m_ipv4_addr; }

      /// Return the set of IPv6 addresses included in this alternative name
      const std::set<IPv6Address>& ipv6_address() const { return m_ipv6_addr; }

      /// Return the set of "other names" included in this alternative name
      BOTAN_DEPRECATED("Support for other names is deprecated")
      const std::set<std::pair<OID, ASN1_String>>& other_names() const {
         return m_othernames;
      }

      /// Return the set of directory names included in this alternative name
      const std::set<X509_DN>& directory_names() const { return m_dn_names; }

      /// Return the total number of names in this AlternativeName
      ///
      /// This only counts names which were parsed, ignoring names which
      /// were of some unknown type
      size_t count() const;

      /// Return true if this has any names set
      bool has_items() const;

      // Old, now deprecated interface follows:
      BOTAN_DEPRECATED("Use AlternativeName::{uris, email, dns, othernames, directory_names}")
      std::multimap<std::string, std::string> contents() const;

      BOTAN_DEPRECATED("Use AlternativeName::{uris, email, dns, othernames, directory_names}.empty()")
      bool has_field(std::string_view attr) const;

      BOTAN_DEPRECATED("Use AlternativeName::{uris, email, dns, othernames, directory_names}")
      std::vector<std::string> get_attribute(std::string_view attr) const;

      BOTAN_DEPRECATED("Use AlternativeName::{uris, email, dns, othernames, directory_names}")
      std::multimap<std::string, std::string, std::less<>> get_attributes() const;

      BOTAN_DEPRECATED("Use AlternativeName::{uris, email, dns, othernames, directory_names}")
      std::string get_first_attribute(std::string_view attr) const;

      BOTAN_DEPRECATED("Use AlternativeName::add_{uri, dns, email, ...}")
      void add_attribute(std::string_view type, std::string_view value);

      BOTAN_DEPRECATED("Use AlternativeName::add_other_name")
      void add_othername(const OID& oid, std::string_view value, ASN1_Type type);

      BOTAN_DEPRECATED("Use AlternativeName::othernames") std::multimap<OID, ASN1_String> get_othernames() const;

      BOTAN_DEPRECATED("Use AlternativeName::directory_names") X509_DN dn() const;

      BOTAN_DEPRECATED("Use plain constructor plus add_{uri,dns,email,ipv4_address}")
      BOTAN_FUTURE_EXPLICIT AlternativeName(std::string_view email_addr,
                                            std::string_view uri = "",
                                            std::string_view dns = "",
                                            std::string_view ip_address = "");

   private:
      std::set<std::string> m_dns;
      std::set<std::string> m_uri;
      std::set<std::string> m_email;
      std::set<uint32_t> m_ipv4_addr;
      std::set<IPv6Address> m_ipv6_addr;
      std::set<X509_DN> m_dn_names;
      std::set<std::pair<OID, ASN1_String>> m_othernames;
};

/**
* Attribute
*/
class BOTAN_PUBLIC_API(2, 0) Attribute final : public ASN1_Object {
   public:
      void encode_into(DER_Encoder& to) const override;
      void decode_from(BER_Decoder& from) override;

      Attribute() = default;
      Attribute(const OID& oid, const std::vector<uint8_t>& params);
      Attribute(std::string_view oid_str, const std::vector<uint8_t>& params);

      const OID& oid() const { return m_oid; }

      const std::vector<uint8_t>& parameters() const { return m_parameters; }

      const OID& object_identifier() const { return m_oid; }

      const std::vector<uint8_t>& get_parameters() const { return m_parameters; }

   private:
      OID m_oid;
      std::vector<uint8_t> m_parameters;
};

/**
* @brief X.509 GeneralName Type
*
* Handles parsing GeneralName types in their BER and canonical string
* encoding. Allows matching GeneralNames against each other using
* the rules laid out in the RFC 5280, sec. 4.2.1.10 (Name Constraints).
*
* This entire class is deprecated and will be removed in a future
* major release
*/
class BOTAN_PUBLIC_API(2, 0) GeneralName final : public ASN1_Object {
   public:
      enum MatchResult : uint8_t /* NOLINT(*-use-enum-class) */ {
         All,
         Some,
         None,
         NotFound,
         UnknownType,
      };

      enum class NameType : uint8_t {
         Unknown = 0,
         RFC822 = 1,
         DNS = 2,
         URI = 3,
         DN = 4,
         IPv4 = 5,
         IPv6 = 6,
         Other = 7,
      };

      BOTAN_DEPRECATED("Deprecated use NameConstraints") GeneralName() = default;

      static GeneralName email(std::string_view email);
      static GeneralName dns(std::string_view dns);
      static GeneralName uri(std::string_view uri);
      static GeneralName directory_name(Botan::X509_DN dn);
      static GeneralName ipv4_address(uint32_t ipv4);
      static GeneralName ipv4_address(uint32_t ipv4, uint32_t mask);
      static GeneralName ipv4_address(IPv4Address ipv4);
      static GeneralName ipv4_address(const IPv4Subnet& subnet);
      static GeneralName ipv6_address(const IPv6Address& ipv6);
      static GeneralName ipv6_address(const IPv6Subnet& subnet);

      // Encoding is not implemented
      void encode_into(DER_Encoder& to) const override;

      void decode_from(BER_Decoder& from) override;

      /**
      * @return Type of the name expressed in this restriction
      */
      NameType type_code() const { return m_type; }

      /**
      * @return Type of the name. Can be DN, DNS, IP, RFC822 or URI.
      */
      BOTAN_DEPRECATED("Deprecated use type_code") std::string type() const;

      /**
      * @return The name as string. Format depends on type.
      */
      BOTAN_DEPRECATED("Deprecated no replacement") std::string name() const;

      /**
      * @return The name as binary string. Format depends on type.
      */
      BOTAN_DEPRECATED("Deprecated no replacement") std::vector<uint8_t> binary_name() const;

      /**
      * Checks whether a given certificate (partially) matches this name.
      * @param cert certificate to be matched
      * @return the match result
      */
      BOTAN_DEPRECATED("Deprecated use NameConstraints type") MatchResult matches(const X509_Certificate& cert) const;

      bool matches_dns(const std::string& dns_name) const;
      bool matches_ipv4(uint32_t ip) const;

      bool matches_ipv4(IPv4Address ip) const { return matches_ipv4(ip.value()); }

      bool matches_ipv6(const IPv6Address& ip) const;
      bool matches_dn(const X509_DN& dn) const;

   private:
      friend class NameConstraints;
      static constexpr size_t RFC822_IDX = 0;
      static constexpr size_t DNS_IDX = 1;
      static constexpr size_t URI_IDX = 2;
      static constexpr size_t DN_IDX = 3;
      static constexpr size_t IPV4_IDX = 4;
      static constexpr size_t IPV6_IDX = 5;

      using NameVariant = std::variant<std::string, std::string, std::string, X509_DN, IPv4Subnet, IPv6Subnet>;

      GeneralName(NameType type, NameVariant name) : m_type(type), m_name(std::move(name)) {}

      template <size_t idx, typename T>
         requires(idx < 6)
      static GeneralName make(T&& value) {
         return {NameType(idx + 1 /* implicit enum relationship! */),
                 NameVariant(std::in_place_index_t<idx>(), std::forward<T>(value))};
      }

      NameType m_type = NameType::Unknown;
      NameVariant m_name;

      static bool matches_dns(std::string_view name, std::string_view constraint);

      /**
      * Partial DN matching according to RFC 5280, Section 7.1, i.e.,
      * whether the constraint is a prefix of the name.
      */
      static bool matches_dn(const X509_DN& name, const X509_DN& constraint);
};

BOTAN_DEPRECATED("Deprecated no replacement") std::ostream& operator<<(std::ostream& os, const GeneralName& gn);

/**
* @brief A single Name Constraint
*
* The Name Constraint extension adds a minimum and maximum path
* length to a GeneralName to form a constraint. The length limits
* are not used in PKIX.
*
* This entire class is deprecated and will be removed in a future
* major release
*/
class BOTAN_PUBLIC_API(2, 0) GeneralSubtree final : public ASN1_Object {
   public:
      /**
      * Creates an empty name constraint.
      */
      BOTAN_DEPRECATED("Deprecated use NameConstraints") GeneralSubtree();

      void encode_into(DER_Encoder& to) const override;

      void decode_from(BER_Decoder& from) override;

      /**
      * @return name
      */
      const GeneralName& base() const { return m_base; }

   private:
      GeneralName m_base;
};

BOTAN_DEPRECATED("Deprecated no replacement") std::ostream& operator<<(std::ostream& os, const GeneralSubtree& gs);

/**
* @brief Name Constraints
*
* Wraps the Name Constraints associated with a certificate.
*/
class BOTAN_PUBLIC_API(2, 0) NameConstraints final {
   public:
      /**
      * Creates an empty name NameConstraints.
      */
      NameConstraints() = default;

      /**
      * Creates NameConstraints from a list of permitted and excluded subtrees.
      * @param permitted_subtrees names for which the certificate is permitted
      * @param excluded_subtrees names for which the certificate is not permitted
      */
      NameConstraints(std::vector<GeneralSubtree>&& permitted_subtrees,
                      std::vector<GeneralSubtree>&& excluded_subtrees);

      /**
      * @return permitted names
      */
      BOTAN_DEPRECATED("Deprecated no replacement") const std::vector<GeneralSubtree>& permitted() const {
         return m_permitted_subtrees;
      }

      /**
      * @return excluded names
      */
      BOTAN_DEPRECATED("Deprecated no replacement") const std::vector<GeneralSubtree>& excluded() const {
         return m_excluded_subtrees;
      }

      /**
      * Return true if all of the names in the certificate are permitted
      */
      bool is_permitted(const X509_Certificate& cert, bool reject_unknown) const;

      /**
      * Return true if any of the names in the certificate are excluded
      */
      bool is_excluded(const X509_Certificate& cert, bool reject_unknown) const;

   private:
      std::vector<GeneralSubtree> m_permitted_subtrees;
      std::vector<GeneralSubtree> m_excluded_subtrees;

      std::set<GeneralName::NameType> m_permitted_name_types;
      std::set<GeneralName::NameType> m_excluded_name_types;
};

/**
* X.509 Certificate Extension
*/
class BOTAN_PUBLIC_API(2, 0) Certificate_Extension /* NOLINT(*-special-member-functions) */ {
   public:
      /**
      * @return OID representing this extension
      */
      virtual OID oid_of() const = 0;

      /*
      * @return specific OID name
      * If possible OIDS table should match oid_name to OIDS, ie
      * OID::from_string(ext->oid_name()) == ext->oid_of()
      * Should return empty string if OID is not known
      */
      virtual std::string oid_name() const = 0;

      /**
      * Make a copy of this extension
      * @return copy of this
      */

      virtual std::unique_ptr<Certificate_Extension> copy() const = 0;

      /*
      * Callback visited during path validation.
      *
      * An extension can implement this callback to inspect
      * the path during path validation.
      *
      * If an error occurs during validation of this extension,
      * an appropriate status code shall be added to cert_status.
      *
      * @param subject Subject certificate that contains this extension
      * @param issuer Issuer certificate. nullopt for certificates with no
      *        available issuer (e.g. non self-signed trust anchors).
      * @param cert_path Certificate path which is currently validated
      * @param cert_status Certificate validation status codes for subject certificate
      * @param pos Position of subject certificate in cert_path
      */
      virtual void validate(const X509_Certificate& subject,
                            const std::optional<X509_Certificate>& issuer,
                            const std::vector<X509_Certificate>& cert_path,
                            std::vector<std::set<Certificate_Status_Code>>& cert_status,
                            size_t pos);

      virtual ~Certificate_Extension() = default;

   protected:
      friend class Extensions;

      virtual bool should_encode() const { return true; }

      virtual std::vector<uint8_t> encode_inner() const = 0;
      virtual void decode_inner(const std::vector<uint8_t>&) = 0;
};

/**
* X.509 Certificate Extension List
*/
class BOTAN_PUBLIC_API(2, 0) Extensions final : public ASN1_Object {
   public:
      /**
      * Look up an object in the extensions, based on OID Returns
      * nullptr if not set, if the extension was either absent or not
      * handled. The pointer returned is owned by the Extensions
      * object.
      * This would be better with an optional<T> return value
      */
      const Certificate_Extension* get_extension_object(const OID& oid) const;

      template <typename T>
      const T* get_extension_object_as(const OID& oid = T::static_oid()) const {
         if(const Certificate_Extension* extn = get_extension_object(oid)) {
            // Unknown_Extension oid_name is empty
            if(extn->oid_name().empty()) {
               return nullptr;
            } else if(const T* extn_as_T = dynamic_cast<const T*>(extn)) {
               return extn_as_T;
            } else {
               throw Decoding_Error("Exception::get_extension_object_as dynamic_cast failed");
            }
         }

         return nullptr;
      }

      /**
      * Return the set of extensions in the order they appeared in the certificate
      * (or as they were added, if constructed)
      */
      const std::vector<OID>& get_extension_oids() const { return m_extension_oids; }

      /**
      * Return the set of critical extensions in the order they appeared in the extension list
      * (This may be an empty vector)
      */
      std::vector<OID> critical_extensions() const;

      /**
      * Return true if an extension was set
      */
      bool extension_set(const OID& oid) const;

      /**
      * Return true if an extension was set and marked critical
      */
      bool critical_extension_set(const OID& oid) const;

      /**
      * Return the raw bytes of the extension
      * Will throw if OID was not set as an extension.
      */
      std::vector<uint8_t> get_extension_bits(const OID& oid) const;

      void encode_into(DER_Encoder& to) const override;
      void decode_from(BER_Decoder& from) override;

      /**
      * Adds a new extension to the list.
      * @param extn pointer to the certificate extension (Extensions takes ownership)
      * @param critical whether this extension should be marked as critical
      * @throw Invalid_Argument if the extension is already present in the list
      */
      void add(std::unique_ptr<Certificate_Extension> extn, bool critical = false);

      /**
      * Adds a new extension to the list unless it already exists. If the extension
      * already exists within the Extensions object, the extn pointer will be deleted.
      *
      * @param extn pointer to the certificate extension (Extensions takes ownership)
      * @param critical whether this extension should be marked as critical
      * @return true if the object was added false if the extension was already used
      */
      bool add_new(std::unique_ptr<Certificate_Extension> extn, bool critical = false);

      /**
      * Adds an extension to the list or replaces it.
      * @param extn the certificate extension
      * @param critical whether this extension should be marked as critical
      */
      void replace(std::unique_ptr<Certificate_Extension> extn, bool critical = false);

      /**
      * Remove an extension from the list. Returns true if the
      * extension had been set, false otherwise.
      */
      bool remove(const OID& oid);

      /**
      * Searches for an extension by OID and returns the result.
      * Only the known extensions types declared in this header
      * are searched for by this function.
      * @return Copy of extension with oid, nullptr if not found.
      * Can avoid creating a copy by using get_extension_object function
      */
      std::unique_ptr<Certificate_Extension> get(const OID& oid) const;

      /**
      * Searches for an extension by OID and returns the result decoding
      * it to some arbitrary extension type chosen by the application.
      *
      * Only the unknown extensions, that is, extensions types that
      * are not declared in this header, are searched for by this
      * function.
      *
      * @return Pointer to new extension with oid, nullptr if not found.
      */
      template <typename T>
      std::unique_ptr<T> get_raw(const OID& oid) const {
         auto extn_info = m_extension_info.find(oid);

         if(extn_info != m_extension_info.end()) {
            // Unknown_Extension oid_name is empty
            if(extn_info->second.obj().oid_name().empty()) {
               auto ext = std::make_unique<T>();
               ext->decode_inner(extn_info->second.bits());
               return ext;
            }
         }
         return nullptr;
      }

      /**
      * Returns a copy of the list of extensions together with the corresponding
      * criticality flag. All extensions are encoded as some object, falling back
      * to Unknown_Extension class which simply allows reading the bytes as well
      * as the criticality flag.
      */
      std::vector<std::pair<std::unique_ptr<Certificate_Extension>, bool>> extensions() const;

      /**
      * Returns the list of extensions as raw, encoded bytes
      * together with the corresponding criticality flag.
      * Contains all extensions, including any extensions encoded as Unknown_Extension
      */
      std::map<OID, std::pair<std::vector<uint8_t>, bool>> extensions_raw() const;

      Extensions() = default;

      Extensions(const Extensions&) = default;
      Extensions& operator=(const Extensions&) = default;

      Extensions(Extensions&&) = default;
      Extensions& operator=(Extensions&&) = default;

      ~Extensions() override = default;

   private:
      static std::unique_ptr<Certificate_Extension> create_extn_obj(const OID& oid,
                                                                    bool critical,
                                                                    const std::vector<uint8_t>& body);

      class BOTAN_UNSTABLE_API Extensions_Info final {
         public:
            Extensions_Info(bool critical, std::unique_ptr<Certificate_Extension> ext) :
                  m_obj(std::move(ext)), m_bits(m_obj->encode_inner()), m_critical(critical) {}

            Extensions_Info(bool critical,
                            const std::vector<uint8_t>& encoding,
                            std::unique_ptr<Certificate_Extension> ext) :
                  m_obj(std::move(ext)), m_bits(encoding), m_critical(critical) {}

            bool is_critical() const { return m_critical; }

            const std::vector<uint8_t>& bits() const { return m_bits; }

            const Certificate_Extension& obj() const;

         private:
            std::shared_ptr<Certificate_Extension> m_obj;
            std::vector<uint8_t> m_bits;
            bool m_critical = false;
      };

      std::vector<OID> m_extension_oids;
      std::map<OID, Extensions_Info> m_extension_info;
};

}  // namespace Botan

namespace Botan {

class Public_Key;
class Private_Key;
class RandomNumberGenerator;
class PK_Signer;

/**
* This class represents abstract X.509 signed objects as in the X.500
* SIGNED macro
*/
class BOTAN_PUBLIC_API(2, 0) X509_Object : public ASN1_Object {
   public:
      /**
      * The underlying data that is to be or was signed
      * @return data that is or was signed
      */
      std::vector<uint8_t> tbs_data() const;

      /**
      * @return signature on tbs_data()
      */
      const std::vector<uint8_t>& signature() const;

      /**
      * @return signed body
      */
      const std::vector<uint8_t>& signed_body() const;

      /**
      * @return signature algorithm that was used to generate signature
      */
      const AlgorithmIdentifier& signature_algorithm() const;

      /**
      * Create a signed X509 object.
      * @param signer the signer used to sign the object
      * @param rng the random number generator to use
      * @param alg_id the algorithm identifier of the signature scheme
      * @param tbs the tbs bits to be signed
      * @return signed X509 object
      */
      static std::vector<uint8_t> make_signed(PK_Signer& signer,
                                              RandomNumberGenerator& rng,
                                              const AlgorithmIdentifier& alg_id,
                                              std::span<const uint8_t> tbs);

      /**
      * Check the signature on this data
      * @param key the public key purportedly used to sign this data
      * @return status of the signature - OK if verified or otherwise an indicator of
      *         the problem preventing verification, along with the hash function that
      *         was used, for further policy checks. The second parameter is empty
      *         unless the validation was successful.
      */
      std::pair<Certificate_Status_Code, std::string> verify_signature(const Public_Key& key) const;

      /**
      * Check the signature on this data
      * @param key the public key purportedly used to sign this data
      * @return true if the signature is valid, otherwise false
      */
      bool check_signature(const Public_Key& key) const;

      /**
      * DER encode an X509_Object
      * See @ref ASN1_Object::encode_into()
      */
      void encode_into(DER_Encoder& to) const override;

      /**
      * Decode a BER encoded X509_Object
      * See @ref ASN1_Object::decode_from()
      */
      void decode_from(BER_Decoder& from) override;

      /**
      * @return PEM encoding of this
      */
      std::string PEM_encode() const;

      virtual std::string PEM_label() const = 0;

      virtual std::vector<std::string> alternate_PEM_labels() const { return std::vector<std::string>(); }

      /**
      * Choose and return a signature scheme appropriate for X.509 signing
      * using the provided parameters.
      *
      * @param key will be the key to choose a padding scheme for
      * @param rng the random generator to use
      * @param hash_fn is the desired hash function
      * @param padding_algo specifies the padding method
      * @return a PK_Signer object for generating signatures
      */
      static std::unique_ptr<PK_Signer> choose_sig_format(const Private_Key& key,
                                                          RandomNumberGenerator& rng,
                                                          std::string_view hash_fn,
                                                          std::string_view padding_algo);

   protected:
      X509_Object() = default;

      /**
      * Decodes from src as either DER or PEM data, then calls force_decode()
      */
      void load_data(DataSource& src);

   private:
      virtual void force_decode() = 0;

      class Signed_Data final {
         public:
            AlgorithmIdentifier m_sig_algo;
            std::vector<uint8_t> m_tbs_bits;
            std::vector<uint8_t> m_sig;
      };

      std::shared_ptr<const Signed_Data> m_signed_data;
};

}  // namespace Botan

namespace Botan {

class Extensions;
class X509_Certificate;
class X509_DN;

class CRL_Entry_Data;
class CRL_Data;

/**
* This class represents CRL entries
*/
class BOTAN_PUBLIC_API(2, 0) CRL_Entry final : public ASN1_Object {
   public:
      void encode_into(DER_Encoder& to) const override;
      void decode_from(BER_Decoder& from) override;

      /**
      * Get the serial number of the certificate associated with this entry.
      * @return certificate's serial number
      */
      const std::vector<uint8_t>& serial_number() const;

      /**
      * Get the revocation date of the certificate associated with this entry
      * @return certificate's revocation date
      */
      const X509_Time& expire_time() const;

      /**
      * Get the entries reason code
      * @return reason code
      */
      CRL_Code reason_code() const;

      /**
      * Get the extensions on this CRL entry
      */
      const Extensions& extensions() const;

      /**
      * Create uninitialized CRL_Entry object
      */
      CRL_Entry() = default;

      /**
      * Construct an CRL entry.
      * @param cert the certificate to revoke
      * @param reason the reason code to set in the entry
      */
      BOTAN_FUTURE_EXPLICIT CRL_Entry(const X509_Certificate& cert, CRL_Code reason = CRL_Code::Unspecified);

   private:
      friend class X509_CRL;

      const CRL_Entry_Data& data() const;

      std::shared_ptr<const CRL_Entry_Data> m_data;
};

/**
* Test two CRL entries for equality in all fields.
*/
BOTAN_PUBLIC_API(2, 0) bool operator==(const CRL_Entry& lhs, const CRL_Entry& rhs);

/**
* Test two CRL entries for inequality in at least one field.
*/
BOTAN_PUBLIC_API(2, 0) bool operator!=(const CRL_Entry& lhs, const CRL_Entry& rhs);

/**
* This class represents X.509 Certificate Revocation Lists (CRLs).
*/
class BOTAN_PUBLIC_API(2, 0) X509_CRL final : public X509_Object {
   public:
      /**
      * Check if this particular certificate is listed in the CRL
      */
      bool is_revoked(const X509_Certificate& cert) const;

      /**
      * Get the entries of this CRL in the form of a vector.
      * @return vector containing the entries of this CRL.
      */
      const std::vector<CRL_Entry>& get_revoked() const;

      /**
      * Get the X509 version of this CRL object
      * @return X509 version
      */
      uint32_t x509_version() const;

      /**
      * Get the issuer DN of this CRL.
      * @return CRLs issuer DN
      */
      const X509_DN& issuer_dn() const;

      /**
      * @return extension data for this CRL
      */
      const Extensions& extensions() const;

      /**
      * Get the AuthorityKeyIdentifier of this CRL.
      * @return this CRLs AuthorityKeyIdentifier
      */
      const std::vector<uint8_t>& authority_key_id() const;

      /**
      * Get the serial number of this CRL.
      * @return CRLs serial number
      */
      uint32_t crl_number() const;

      /**
      * Get the CRL's thisUpdate value.
      * @return CRLs thisUpdate
      */
      const X509_Time& this_update() const;

      /**
      * Get the CRL's nextUpdate value.
      *
      * Technically nextUpdate is optional in the X.509 spec and may be omitted,
      * despite RFC 5280 requiring it. If the nextUpdate field is not set, this
      * will return a time object with time_is_set() returning false.
      *
      * TODO(Botan4) return a `const std::optional<X509_Time>&` instead
      *
      * @return CRLs nextUpdate
      */
      const X509_Time& next_update() const;

      /**
      * Get the CRL's issuing distribution point
      */
      BOTAN_DEPRECATED("Use issuing_distribution_points") std::string crl_issuing_distribution_point() const;

      /**
      * Get the CRL's issuing distribution points
      *
      * See https://www.rfc-editor.org/rfc/rfc5280#section-5.2.5
      */
      std::vector<std::string> issuing_distribution_points() const;

      /**
      * Check if this CRL's scope covers the given certificate's CRL distribution points.
      *
      * Per RFC 5280 6.3.3 step (b)(2), if the certificate has a CRL Distribution Points
      * extension (4.2.1.13) and this CRL has an Issuing Distribution Point extension
      * (5.2.5), at least one general name from the IDP must match a general name in one
      * of the certificate's distribution points.
      *
      * Returns true if the certificate has no CRLDP extension (this CRL's scope is
      * unconstrained from the certificate's perspective), or if both extensions are
      * present and their distribution point names overlap. Returns false otherwise,
      * including when the certificate has a CRLDP but this CRL has no IDP.
      *
      * The nameRelativeToCRLIssuer RDN form of DistributionPointName is not currently
      * parsed by Botan's CRLDP/IDP decoders, so this comparison operates only on the
      * fullName (GeneralNames) form.
      */
      bool has_matching_distribution_point(const X509_Certificate& cert) const;

      /**
      * Create an uninitialized CRL object. Any attempts to access
      * this object will throw an exception.
      */
      X509_CRL() = default;

      /**
      * Construct a CRL from a data source.
      * @param source the data source providing the DER or PEM encoded CRL.
      */
      BOTAN_FUTURE_EXPLICIT X509_CRL(DataSource& source);

#if defined(BOTAN_TARGET_OS_HAS_FILESYSTEM)
      /**
      * Construct a CRL from a file containing the DER or PEM encoded CRL.
      * @param filename the name of the CRL file
      */
      BOTAN_FUTURE_EXPLICIT X509_CRL(std::string_view filename);
#endif

      /**
      * Construct a CRL from a binary vector
      * @param vec the binary (DER) representation of the CRL
      */
      BOTAN_FUTURE_EXPLICIT X509_CRL(const std::vector<uint8_t>& vec);

      /**
      * Construct a CRL
      * @param issuer issuer of this CRL
      * @param thisUpdate valid from
      * @param nextUpdate valid until
      * @param revoked entries to be included in the CRL
      */
      X509_CRL(const X509_DN& issuer,
               const X509_Time& thisUpdate,
               const X509_Time& nextUpdate,
               const std::vector<CRL_Entry>& revoked);

   private:
      std::string PEM_label() const override;

      std::vector<std::string> alternate_PEM_labels() const override;

      void force_decode() override;

      const CRL_Data& data() const;

      std::shared_ptr<const CRL_Data> m_data;
};

}  // namespace Botan

namespace Botan {

class AlternativeName;
class Extensions;
class NameConstraints;
class Public_Key;
class X509_DN;

class X509_Certificate_Data;

/**
* This class represents an X.509 Certificate
*
* TODO(Botan4) mark this final once PKCS11_X509_Certificate is fixed
*/
class BOTAN_PUBLIC_API(2, 0) X509_Certificate : public X509_Object {
   public:
      /**
      * Create a public key object associated with the public key bits in this
      * certificate. If the public key bits was valid for X.509 encoding
      * purposes but invalid algorithmically (for example, RSA with an even
      * modulus) that will be detected at this point, and an exception will be
      * thrown.
      *
      * @return subject public key of this certificate
      */
      std::unique_ptr<Public_Key> subject_public_key() const;

      /**
      * Create a public key object associated with the public key bits in this
      * certificate. If the public key bits was valid for X.509 encoding
      * purposes but invalid algorithmically (for example, RSA with an even
      * modulus) that will be detected at this point, and an exception will be
      * thrown.
      *
      * @return subject public key of this certificate
      */
      BOTAN_DEPRECATED("Use subject_public_key") std::unique_ptr<Public_Key> load_subject_public_key() const;

      /**
      * Get the public key associated with this certificate. This includes the
      * outer AlgorithmIdentifier
      * @return subject public key of this certificate
      */
      const std::vector<uint8_t>& subject_public_key_bits() const;

      /**
      * Get the SubjectPublicKeyInfo associated with this certificate.
      * @return subject public key info of this certificate
      */
      const std::vector<uint8_t>& subject_public_key_info() const;

      /**
      * Return the algorithm identifier of the public key
      */
      const AlgorithmIdentifier& subject_public_key_algo() const;

      /**
      * Get the bit string of the public key associated with this certificate
      * @return public key bits
      */
      const std::vector<uint8_t>& subject_public_key_bitstring() const;

      /**
      * Get the SHA-1 bit string of the public key associated with this certificate.
      * This is used for OCSP among other protocols.
      * This function will throw if SHA-1 is not available.
      * @return hash of subject public key of this certificate
      */
      const std::vector<uint8_t>& subject_public_key_bitstring_sha1() const;

      /**
      * Get the certificate's issuer distinguished name (DN).
      * @return issuer DN of this certificate
      */
      const X509_DN& issuer_dn() const;

      /**
      * Get the certificate's subject distinguished name (DN).
      * @return subject DN of this certificate
      */
      const X509_DN& subject_dn() const;

      /**
      * Get a value for a specific subject_info parameter name.
      * @param name the name of the parameter to look up. Possible names include
      * "X509.Certificate.version", "X509.Certificate.serial",
      * "X509.Certificate.start", "X509.Certificate.end",
      * "X509.Certificate.v2.key_id", "X509.Certificate.public_key",
      * "X509v3.BasicConstraints.path_constraint",
      * "X509v3.BasicConstraints.is_ca", "X509v3.NameConstraints",
      * "X509v3.ExtendedKeyUsage", "X509v3.CertificatePolicies",
      * "X509v3.SubjectKeyIdentifier", "X509.Certificate.serial",
      * "X520.CommonName", "X520.Organization", "X520.Country",
      * "RFC822" (Email in SAN) or "PKCS9.EmailAddress" (Email in DN).
      * @return value(s) of the specified parameter
      */
      std::vector<std::string> subject_info(std::string_view name) const;

      /**
      * Get a value for a specific subject_info parameter name.
      * @param name the name of the parameter to look up. Possible names are
      * "X509.Certificate.v2.key_id" or "X509v3.AuthorityKeyIdentifier".
      * @return value(s) of the specified parameter
      */
      std::vector<std::string> issuer_info(std::string_view name) const;

      /**
      * Raw issuer DN bits
      */
      const std::vector<uint8_t>& raw_issuer_dn() const;

      /**
      * SHA-256 of Raw issuer DN
      */
      const std::vector<uint8_t>& raw_issuer_dn_sha256() const;

      /**
      * Raw subject DN
      */
      const std::vector<uint8_t>& raw_subject_dn() const;

      /**
      * SHA-256 of Raw subject DN
      */
      const std::vector<uint8_t>& raw_subject_dn_sha256() const;

      /**
      * SHA-1 of the entire certificate DER encoding
      */
      std::span<const uint8_t, 20> certificate_data_sha1() const;

      /**
      * SHA-256 of the entire certificate DER encoding
      */
      std::span<const uint8_t, 32> certificate_data_sha256() const;

      /**
      * Get the notBefore of the certificate as X509_Time
      * @return notBefore of the certificate
      */
      const X509_Time& not_before() const;

      /**
      * Get the notAfter of the certificate as X509_Time
      * @return notAfter of the certificate
      */
      const X509_Time& not_after() const;

      /**
      * Get the X509 version of this certificate object.
      * @return X509 version
      */
      uint32_t x509_version() const;

      /**
      * Get the serial number of this certificate.
      * @return certificates serial number
      */
      const std::vector<uint8_t>& serial_number() const;

      /**
      * Get the serial number's sign
      * @return 1 iff the serial is negative.
      */
      bool is_serial_negative() const;

      /**
      * Get the DER encoded AuthorityKeyIdentifier of this certificate.
      * @return DER encoded AuthorityKeyIdentifier
      */
      const std::vector<uint8_t>& authority_key_id() const;

      /**
      * Get the DER encoded SubjectKeyIdentifier of this certificate.
      * @return DER encoded SubjectKeyIdentifier
      */
      const std::vector<uint8_t>& subject_key_id() const;

      /**
      * Check whether this certificate is self signed.
      * If the DN issuer and subject agree,
      * @return true if this certificate is self signed
      */
      bool is_self_signed() const;

      /**
      * Check whether this certificate is a CA certificate.
      * @return true if this certificate is a CA certificate
      */
      bool is_CA_cert() const;

      /**
      * Returns true if the specified @param usage is set in the key usage extension
      * or if no key usage constraints are set at all.
      * To check if a certain key constraint is set in the certificate
      * use @see X509_Certificate#has_constraints.
      */
      bool allowed_usage(Key_Constraints usage) const;

      /**
      * Returns true if the specified @param usage is set in the extended key usage extension
      * or if no extended key usage constraints are set at all.
      * To check if a certain extended key constraint is set in the certificate
      * use @see X509_Certificate#has_ex_constraint.
      */
      bool allowed_extended_usage(std::string_view usage) const;

      /**
      * Returns true if the specified usage is set in the extended key usage extension,
      * or if no extended key usage constraints are set at all.
      * To check if a certain extended key constraint is set in the certificate
      * use @see X509_Certificate#has_ex_constraint.
      */
      bool allowed_extended_usage(const OID& usage) const;

      /**
      * Returns true if the required key and extended key constraints are set in the certificate
      * for the specified @param usage or if no key constraints are set in both the key usage
      * and extended key usage extension.
      */
      bool allowed_usage(Usage_Type usage) const;

      /**
      * Returns true if and only if the specified @param constraints are
      * included in the key usage extension.
      *
      * Typically for applications you want allowed_usage instead.
      */
      bool has_constraints(Key_Constraints constraints) const;

      /**
      * Returns true if and only if OID @param ex_constraint is
      * included in the extended key extension.
      */
      bool has_ex_constraint(std::string_view ex_constraint) const;

      /**
      * Returns true if and only if OID @param ex_constraint is
      * included in the extended key extension.
      */
      bool has_ex_constraint(const OID& ex_constraint) const;

      /**
      * Get the path length constraint as defined in the BasicConstraints extension.
      *
      * This returns an arbitrary value if the extension is not set (either 32 for v1
      * self-signed certificates, or else Cert_Extension::NO_CERT_PATH_LIMIT for v3
      * certificates without the extension)
      *
      * Prefer path_length_constraint
      *
      * @return path limit
      */
      BOTAN_DEPRECATED("Use X509_Certificate::path_length_constraint") uint32_t path_limit() const;

      /**
      * Get the path length constraint as defined in the BasicConstraints extension.
      *
      * Returns nullopt if either the extension is not set in the certificate,
      * or if the pathLenConstraint field was absent from the extension.
      *
      * @return path limit
      */
      std::optional<size_t> path_length_constraint() const;

      /**
      * Check whenever a given X509 Extension is marked critical in this
      * certificate.
      */
      bool is_critical(std::string_view ex_name) const;

      /**
      * Get the key constraints as defined in the KeyUsage extension of this
      * certificate.
      * @return key constraints
      */
      Key_Constraints constraints() const;

      /**
      * Get the key usage as defined in the ExtendedKeyUsage extension
      * of this certificate, or else an empty vector.
      * @return key usage
      */
      const std::vector<OID>& extended_key_usage() const;

      /**
      * Get the name constraints as defined in the NameConstraints
      * extension of this certificate.
      * @return name constraints
      */
      const NameConstraints& name_constraints() const;

      /**
      * Get the policies as defined in the CertificatePolicies extension
      * of this certificate.
      * @return certificate policies
      */
      const std::vector<OID>& certificate_policy_oids() const;

      /**
      * Get all extensions of this certificate.
      * @return certificate extensions
      */
      const Extensions& v3_extensions() const;

      /**
      * Return the v2 issuer key ID. v2 key IDs are almost never used,
      * instead see v3_subject_key_id.
      */
      const std::vector<uint8_t>& v2_issuer_key_id() const;

      /**
      * Return the v2 subject key ID. v2 key IDs are almost never used,
      * instead see v3_subject_key_id.
      */
      const std::vector<uint8_t>& v2_subject_key_id() const;

      /**
      * Return the subject alternative names (DNS, IP, ...)
      */
      const AlternativeName& subject_alt_name() const;

      /**
      * Return the issuer alternative names (DNS, IP, ...)
      */
      const AlternativeName& issuer_alt_name() const;

      /**
      * Return the listed address of an OCSP responder, or empty if not set
      */
      BOTAN_DEPRECATED("Use ocsp_responders") std::string ocsp_responder() const;

      /**
      * Return the listed addresses of OCSP responders, or empty if not set
      */
      const std::vector<std::string>& ocsp_responders() const;

      /**
      * Return the listed addresses of ca issuers, or empty if not set
      */
      std::vector<std::string> ca_issuers() const;

      /**
      * Return the CRL distribution point, or empty if not set
      */
      BOTAN_DEPRECATED("Use crl_distribution_points") std::string crl_distribution_point() const;

      /**
      * Return the CRL distribution points, or empty if not set
      */
      std::vector<std::string> crl_distribution_points() const;

      /**
      * @return a free-form string describing the certificate
      */
      std::string to_string() const;

      /**
      * @return a fingerprint of the certificate
      * @param hash_name hash function used to calculate the fingerprint
      */
      std::string fingerprint(std::string_view hash_name = "SHA-1") const;

      /**
      * A collision resistant binary "tag" of a certificate
      *
      * The actual value is deliberately not exposed; a Tag can only be hashed
      * to a size_t, or compared with another Tag. This type is intended for use
      * as a key in std::map and std::unordered_map, or to be saved in a
      * std::set or std::unordered_set.
      */
      class Tag final {
         public:
            static constexpr size_t TagLen = 32;

            auto operator<=>(const Tag&) const = default;

            size_t hash() const noexcept {
               size_t h = 0;
               std::memcpy(&h, m_tag.data(), sizeof(h));
               return h;
            }

         private:
            friend X509_Certificate;

            explicit Tag(std::array<uint8_t, TagLen> tag) : m_tag(tag) {}

            std::array<std::uint8_t, TagLen> m_tag;
      };

      class TagHash final {
         public:
            size_t operator()(const X509_Certificate::Tag& tag) const noexcept { return tag.hash(); }
      };

      /**
      * Return a collision resistant binary "tag" of this certificate
      */
      Tag tag() const;

      /**
      * Check if a certain DNS name matches up with the information in
      * the cert
      * @param name DNS name to match
      *
      * Note: this will also accept a dotted quad input, in which case
      * the SAN for IPv4 addresses will be checked.
      */
      bool matches_dns_name(std::string_view name) const;

      /**
      * Check to certificates for equality.
      * @return true both certificates are (binary) equal
      */
      bool operator==(const X509_Certificate& other) const;

      /**
      * Impose an arbitrary (but consistent) ordering, eg to allow sorting
      * a container of certificate objects.
      * @return true if this is less than other by some unspecified criteria
      */
      bool operator<(const X509_Certificate& other) const;

      /**
      * Create a certificate from a data source providing the DER or
      * PEM encoded certificate.
      * @param source the data source
      */
      explicit X509_Certificate(DataSource& source);

#if defined(BOTAN_TARGET_OS_HAS_FILESYSTEM)
      /**
      * Create a certificate from a file containing the DER or PEM
      * encoded certificate.
      * @param filename the name of the certificate file
      */
      explicit X509_Certificate(std::string_view filename);
#endif

      /**
      * Create a certificate from a buffer
      * @param in the buffer containing the DER-encoded certificate
      */
      explicit X509_Certificate(std::span<const uint8_t> in);

      /**
      * Create a certificate from a buffer
      * @param data the buffer containing the DER-encoded certificate
      * @param length length of data in bytes
      */
      X509_Certificate(const uint8_t data[], size_t length) : X509_Certificate(std::span{data, length}) {}

      /**
      * Create an uninitialized certificate object. Any attempts to
      * access this object will throw an exception.
      */
      X509_Certificate() = default;

      X509_Certificate(const X509_Certificate& other) = default;
      X509_Certificate(X509_Certificate&& other) = default;
      X509_Certificate& operator=(const X509_Certificate& other) = default;
      X509_Certificate& operator=(X509_Certificate&& other) = default;
      ~X509_Certificate() override;

   private:
      std::string PEM_label() const override;

      std::vector<std::string> alternate_PEM_labels() const override;

      void force_decode() override;

      const X509_Certificate_Data& data() const;

      std::shared_ptr<const X509_Certificate_Data> m_data;
};

/**
* Check two certificates for inequality
* @param cert1 The first certificate
* @param cert2 The second certificate
* @return true if the arguments represent different certificates,
* false if they are binary identical
*/
BOTAN_PUBLIC_API(2, 0) bool operator!=(const X509_Certificate& cert1, const X509_Certificate& cert2);

}  // namespace Botan

namespace Botan {

/**
* Certificate Store Interface
*/
class BOTAN_PUBLIC_API(2, 0) Certificate_Store /* NOLINT(*-special-member-functions) */ {
   public:
      virtual ~Certificate_Store();

      /**
      * Find a certificate by Subject DN and (optionally) key identifier
      * @param subject_dn the subject's distinguished name
      * @param key_id an optional key id
      * @return a matching certificate or nullopt otherwise
      * If more than one certificate in the certificate store matches, then
      * a single value is selected arbitrarily.
      */
      virtual std::optional<X509_Certificate> find_cert(const X509_DN& subject_dn,
                                                        const std::vector<uint8_t>& key_id) const;

      /**
      * Find all certificates with a given Subject DN.
      * Subject DN and even the key identifier might not be unique.
      */
      virtual std::vector<X509_Certificate> find_all_certs(const X509_DN& subject_dn,
                                                           const std::vector<uint8_t>& key_id) const = 0;

      /**
      * Find a certificate by searching for one with a matching SHA-1 hash of
      * public key. Used for OCSP.
      * @param key_hash SHA-1 hash of the subject's public key
      * @return a matching certificate or nullopt otherwise
      */
      virtual std::optional<X509_Certificate> find_cert_by_pubkey_sha1(const std::vector<uint8_t>& key_hash) const = 0;

      /**
      * Find a certificate by searching for one with a matching SHA-256 hash of
      * raw subject name. Used for OCSP.
      * @param subject_hash SHA-256 hash of the subject's raw name
      * @return a matching certificate or nullopt otherwise
      */
      virtual std::optional<X509_Certificate> find_cert_by_raw_subject_dn_sha256(
         const std::vector<uint8_t>& subject_hash) const = 0;

      /**
      * Find a certificate by searching for one with a matching issuer DN and
      * serial number. Used for CMS or PKCS#7.
      * @param issuer_dn the distinguished name of the issuer
      * @param serial_number the certificate's serial number
      * @return a matching certificate or nullopt otherwise
      */
      virtual std::optional<X509_Certificate> find_cert_by_issuer_dn_and_serial_number(
         const X509_DN& issuer_dn, std::span<const uint8_t> serial_number) const = 0;

      /**
      * Finds a CRL for the given certificate
      * @param subject the subject certificate
      * @return the CRL for subject or nullopt otherwise
      */
      virtual std::optional<X509_CRL> find_crl_for(const X509_Certificate& subject) const;

      /**
      * @return whether this certificate is contained within the store
      * @param cert certificate to be searched
      *
      * Default implementation uses find_all_certs
      */
      virtual bool contains(const X509_Certificate& cert) const;

      /**
      * Old version of contains
      */
      bool certificate_known(const X509_Certificate& cert) const;

      // remove this (used by TLS::Server)
      virtual std::vector<X509_DN> all_subjects() const = 0;
};

/**
* In Memory Certificate Store
*/
class BOTAN_PUBLIC_API(2, 0) Certificate_Store_In_Memory final : public Certificate_Store {
   public:
#if defined(BOTAN_TARGET_OS_HAS_FILESYSTEM)
      /**
      * Attempt to parse all files in dir (including subdirectories)
      * as certificates. Ignores errors.
      */
      explicit Certificate_Store_In_Memory(std::string_view dir);
#endif

      /**
      * Adds given certificate to the store.
      */
      explicit Certificate_Store_In_Memory(const X509_Certificate& cert);

      /**
      * Adds given certificate and CRL to the store.
      */
      Certificate_Store_In_Memory(const X509_Certificate& cert, const X509_CRL& crl);

      /**
      * Create an empty store.
      */
      Certificate_Store_In_Memory();

      Certificate_Store_In_Memory(const Certificate_Store_In_Memory& other);
      Certificate_Store_In_Memory(Certificate_Store_In_Memory&& other) noexcept;

      Certificate_Store_In_Memory& operator=(const Certificate_Store_In_Memory& other) = delete;
      Certificate_Store_In_Memory& operator=(Certificate_Store_In_Memory&& other) noexcept;

      ~Certificate_Store_In_Memory() override;

      /**
      * Add a certificate to the store.
      * @param cert certificate to be added
      */
      void add_certificate(const X509_Certificate& cert);

      /**
      * Add a certificate revocation list (CRL) to the store.
      * @param crl CRL to be added
      */
      void add_crl(const X509_CRL& crl);

      /**
      * @return DNs for all certificates managed by the store
      */
      std::vector<X509_DN> all_subjects() const override;

      /*
      * Find a certificate by Subject DN and (optionally) key identifier
      * @return the first certificate that matches
      */
      std::optional<X509_Certificate> find_cert(const X509_DN& subject_dn,
                                                const std::vector<uint8_t>& key_id) const override;

      /*
      * Find all certificates with a given Subject DN.
      * Subject DN and even the key identifier might not be unique.
      */
      std::vector<X509_Certificate> find_all_certs(const X509_DN& subject_dn,
                                                   const std::vector<uint8_t>& key_id) const override;

      std::optional<X509_Certificate> find_cert_by_pubkey_sha1(const std::vector<uint8_t>& key_hash) const override;

      std::optional<X509_Certificate> find_cert_by_raw_subject_dn_sha256(
         const std::vector<uint8_t>& subject_hash) const override;

      std::optional<X509_Certificate> find_cert_by_issuer_dn_and_serial_number(
         const X509_DN& issuer_dn, std::span<const uint8_t> serial_number) const override;

      /**
      * Finds a CRL for the given certificate
      */
      std::optional<X509_CRL> find_crl_for(const X509_Certificate& subject) const override;

      bool contains(const X509_Certificate& cert) const override;

   private:
      class Impl;

      Impl& impl();
      const Impl& impl() const;

      std::unique_ptr<Impl> m_impl;
};

}  // namespace Botan

namespace Botan {

class BOTAN_PUBLIC_API(2, 0) SQL_Database /* NOLINT(*-special-member-functions) */ {
   public:
      class BOTAN_PUBLIC_API(2, 0) SQL_DB_Error final : public Exception {
         public:
            explicit SQL_DB_Error(std::string_view what) : Exception("SQL database", what), m_rc(0) {}

            SQL_DB_Error(std::string_view what, int rc) : Exception("SQL database", what), m_rc(rc) {}

            ErrorType error_type() const noexcept override { return ErrorType::DatabaseError; }

            int error_code() const noexcept override { return m_rc; }

         private:
            int m_rc;
      };

      class BOTAN_PUBLIC_API(2, 0) Statement /* NOLINT(*-special-member-functions) */ {
         public:
            /* Bind statement parameters */
            virtual void bind(int column, std::string_view str) = 0;

            virtual void bind(int column, size_t i) = 0;

            virtual void bind(int column, std::chrono::system_clock::time_point time) = 0;

            virtual void bind(int column, const std::vector<uint8_t>& blob) = 0;

            virtual void bind(int column, const uint8_t* data, size_t len) = 0;

            // TODO(Botan4) change this return type to a span
            /* Get output */
            virtual std::pair<const uint8_t*, size_t> get_blob(int column) = 0;

            virtual std::string get_str(int column) = 0;

            virtual size_t get_size_t(int column) = 0;

            /* Run to completion */
            virtual size_t spin() = 0;

            /* Maybe update */
            virtual bool step() = 0;

            virtual ~Statement() = default;
      };

      /*
      * Create a new statement for execution.
      * Use ?1, ?2, ?3, etc for parameters to set later with bind
      */
      virtual std::shared_ptr<Statement> new_statement(std::string_view base_sql) const = 0;

      virtual size_t row_count(std::string_view table_name) = 0;

      virtual void create_table(std::string_view table_schema) = 0;

      virtual size_t rows_changed_by_last_statement() = 0;

      virtual size_t exec(std::string_view sql) { return new_statement(sql)->spin(); }

      virtual bool is_threadsafe() const { return false; }

      virtual ~SQL_Database() = default;
};

}  // namespace Botan

namespace Botan {

class Private_Key;
class RandomNumberGenerator;

/**
 * Certificate and private key store backed by an SQL database.
 */
class BOTAN_PUBLIC_API(2, 0) Certificate_Store_In_SQL : public Certificate_Store {
   public:
      /**
      * Create/open a certificate store.
      * @param db underlying database storage
      * @param passwd password to encrypt private keys in the database
      * @param rng used for encrypting keys
      * @param table_prefix optional prefix for db table names
      */
      explicit Certificate_Store_In_SQL(std::shared_ptr<SQL_Database> db,
                                        std::string_view passwd,
                                        RandomNumberGenerator& rng,
                                        std::string_view table_prefix = "");

      /**
      * Returns the first certificate with matching subject DN and optional key ID.
      */
      std::optional<X509_Certificate> find_cert(const X509_DN& subject_dn,
                                                const std::vector<uint8_t>& key_id) const override;

      /*
      * Find all certificates with a given Subject DN.
      * Subject DN and even the key identifier might not be unique.
      */
      std::vector<X509_Certificate> find_all_certs(const X509_DN& subject_dn,
                                                   const std::vector<uint8_t>& key_id) const override;

      std::optional<X509_Certificate> find_cert_by_pubkey_sha1(const std::vector<uint8_t>& key_hash) const override;

      std::optional<X509_Certificate> find_cert_by_raw_subject_dn_sha256(
         const std::vector<uint8_t>& subject_hash) const override;

      std::optional<X509_Certificate> find_cert_by_issuer_dn_and_serial_number(
         const X509_DN& issuer_dn, std::span<const uint8_t> serial_number) const override;

      /**
      * Returns all subject DNs known to the store instance.
      */
      std::vector<X509_DN> all_subjects() const override;

      /**
      * Inserts "cert" into the store, returns false if the certificate is
      * already known and true if insertion was successful.
      */
      bool insert_cert(const X509_Certificate& cert);

      /**
      * Removes "cert" from the store. Returns false if the certificate could not
      * be found and true if removal was successful.
      */
      bool remove_cert(const X509_Certificate& cert);

      /// Returns the private key for "cert" or an empty shared_ptr if none was found.
      std::shared_ptr<const Private_Key> find_key(const X509_Certificate& cert) const;

      /// Returns all certificates for private key "key".
      std::vector<X509_Certificate> find_certs_for_key(const Private_Key& key) const;

      /**
      * Inserts "key" for "cert" into the store, returns false if the key is
      * already known and true if insertion was successful.
      */
      bool insert_key(const X509_Certificate& cert, const Private_Key& key);

      /// Removes "key" from the store.
      void remove_key(const Private_Key& key);

      /// Marks "cert" as revoked starting from "time".
      void revoke_cert(const X509_Certificate& cert, CRL_Code reason, const X509_Time& time);

      /// Marks "cert" as revoked with no time specified
      void revoke_cert(const X509_Certificate& cert, CRL_Code reason);

      /// Reverses the revocation for "cert".
      void affirm_cert(const X509_Certificate& cert);

      /**
      * Generates Certificate Revocation Lists for all certificates marked as revoked.
      * A CRL is returned for each unique issuer DN.
      */
      std::vector<X509_CRL> generate_crls() const;

      /**
      * Generates a CRL for all certificates issued by the given issuer.
      */
      std::optional<X509_CRL> find_crl_for(const X509_Certificate& issuer) const override;

      bool contains(const X509_Certificate& cert) const override;

   private:
      RandomNumberGenerator& m_rng;
      std::shared_ptr<SQL_Database> m_database;
      std::string m_prefix;
      std::string m_password;
};

}  // namespace Botan

namespace Botan {

/**
* Certificate and private key store backed by an sqlite (https://sqlite.org) database.
*/
class BOTAN_PUBLIC_API(2, 0) Certificate_Store_In_SQLite final : public Certificate_Store_In_SQL {
   public:
      /**
      * Create/open a certificate store.
      * @param db_path path to the database file
      * @param passwd password to encrypt private keys in the database
      * @param rng used for encrypting keys
      * @param table_prefix optional prefix for db table names
      */
      Certificate_Store_In_SQLite(std::string_view db_path,
                                  std::string_view passwd,
                                  RandomNumberGenerator& rng,
                                  std::string_view table_prefix = "");
};
}  // namespace Botan

namespace Botan {

/**
* The two possible directions a Cipher_Mode can operate in
*/
enum class Cipher_Dir : uint8_t {
   Encryption = 0,
   Decryption = 1,

   ENCRYPTION BOTAN_DEPRECATED("Use Cipher_Dir::Encryption") = Encryption,
   DECRYPTION BOTAN_DEPRECATED("Use Cipher_Dir::Decryption") = Decryption,
};

/**
* Interface for cipher modes
*/
class BOTAN_PUBLIC_API(2, 0) Cipher_Mode : public SymmetricAlgorithm {
   public:
      /**
      * @return list of available providers for this algorithm, empty if not available
      * @param algo_spec algorithm name
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      /**
      * Create an AEAD mode
      * @param algo the algorithm to create
      * @param direction specify if this should be an encryption or decryption AEAD
      * @param provider optional specification for provider to use
      * @return an AEAD mode or a null pointer if not available
      */
      static std::unique_ptr<Cipher_Mode> create(std::string_view algo,
                                                 Cipher_Dir direction,
                                                 std::string_view provider = "");

      /**
      * Create an AEAD mode, or throw
      * @param algo the algorithm to create
      * @param direction specify if this should be an encryption or decryption AEAD
      * @param provider optional specification for provider to use
      * @return an AEAD mode, or throw an exception
      */
      static std::unique_ptr<Cipher_Mode> create_or_throw(std::string_view algo,
                                                          Cipher_Dir direction,
                                                          std::string_view provider = "");

   protected:
      /*
      * Prepare for processing a message under the specified nonce
      */
      virtual void start_msg(const uint8_t nonce[], size_t nonce_len) = 0;

      /*
      * Process message blocks
      * Input must be a multiple of update_granularity.
      */
      virtual size_t process_msg(uint8_t msg[], size_t msg_len) = 0;

      /*
      * Finishes a message
      */
      virtual void finish_msg(secure_vector<uint8_t>& final_block, size_t offset = 0) = 0;

   public:
      /**
      * Begin processing a message with a fresh nonce.
      *
      * @warning Typically one must not reuse the same nonce for more than one
      *          message under the same key. For most cipher modes this would
      *          mean total loss of security and/or authenticity guarantees.
      *
      * @note If reliably generating unique nonces is difficult in your
      *       environment, use SIV which retains security even if nonces
      *       are repeated.
      *
      * @param nonce the per message nonce
      */
      void start(std::span<const uint8_t> nonce) { start_msg(nonce.data(), nonce.size()); }

      /**
      * Begin processing a message with a fresh nonce.
      * @param nonce the per message nonce
      * @param nonce_len length of nonce
      */
      void start(const uint8_t nonce[], size_t nonce_len) { start_msg(nonce, nonce_len); }

      /**
      * Begin processing a message.
      *
      * The exact semantics of this depend on the mode. For many modes, the call
      * will fail since a nonce must be provided.
      *
      * For certain modes such as CBC this will instead cause the last
      * ciphertext block to be used as the nonce of the new message; doing this
      * isn't a good idea, but some (mostly older) protocols do this.
      */
      void start() { return start_msg(nullptr, 0); }

      /**
      * Process message blocks
      *
      * Input must be a multiple of update_granularity
      *
      * Processes msg in place and returns bytes written. Normally
      * this will be either msg_len (indicating the entire message was
      * processed) or for certain AEAD modes zero (indicating that the
      * mode requires the entire message be processed in one pass).
      *
      * @param msg the message to be processed
      * @return bytes written in-place
      */
      size_t process(std::span<uint8_t> msg) { return this->process_msg(msg.data(), msg.size()); }

      size_t process(uint8_t msg[], size_t msg_len) { return this->process_msg(msg, msg_len); }

      /**
      * Process some data. Input must be in size update_granularity() uint8_t
      * blocks. The @p buffer is an in/out parameter and may be resized. In
      * particular, some modes require that all input be consumed before any
      * output is produced; with these modes, @p buffer will be returned empty.
      *
      * The first @p offset bytes of @p buffer will be ignored (this allows in
      * place processing of a buffer that contains an initial plaintext header).
      *
      * @param buffer in/out parameter which will possibly be resized
      * @param offset an offset into blocks to begin processing
      */
      template <concepts::resizable_byte_buffer T>
      void update(T& buffer, size_t offset = 0) {
         const size_t written = process(std::span(buffer).subspan(offset));
         buffer.resize(offset + written);
      }

      /**
      * Complete procession of a message with a final input of @p buffer, which
      * is treated the same as with update(). If you have the entire message in
      * hand, calling finish() without ever calling update() is both efficient
      * and convenient.
      *
      * When using an AEAD_Mode, if the supplied authentication tag does not
      * validate, this will throw an instance of Invalid_Authentication_Tag.
      *
      * If this occurs, all plaintext previously output via calls to update must
      * be destroyed and not used in any way that an attacker could observe the
      * effects of. This could be anything from echoing the plaintext back
      * (perhaps in an error message), or by making an external RPC whose
      * destination or contents depend on the plaintext. The only thing you can
      * do is buffer it, and in the event of an invalid tag, erase the
      * previously decrypted content from memory.
      *
      * One simple way to assure this could never happen is to never call
      * update, and instead always marshal the entire message into a single
      * buffer and call finish on it when decrypting.
      *
      * @param final_block in/out parameter which must be at least
      *        minimum_final_size() bytes, and will be set to any final output
      * @param offset an offset into final_block to begin processing
      */
      void finish(secure_vector<uint8_t>& final_block, size_t offset = 0) { finish_msg(final_block, offset); }

      /**
      * Complete procession of a message.
      *
      * Note: Using this overload with anything but a Botan::secure_vector<>
      *       is copying the bytes in the in/out buffer.
      *
      * @param final_block in/out parameter which must be at least
      *        minimum_final_size() bytes, and will be set to any final output
      * @param offset an offset into final_block to begin processing
      */
      template <concepts::resizable_byte_buffer T>
      void finish(T& final_block, size_t offset = 0) {
         Botan::secure_vector<uint8_t> tmp(final_block.begin(), final_block.end());
         finish_msg(tmp, offset);
         final_block.resize(tmp.size());
         std::copy(tmp.begin(), tmp.end(), final_block.begin());
      }

      /**
      * Returns the size of the output if this transform is used to process a
      * message with input_length bytes. In most cases the answer is precise.
      * If it is not possible to precise (namely for CBC decryption) instead an
      * upper bound is returned.
      */
      virtual size_t output_length(size_t input_length) const = 0;

      /**
      * The :cpp:class:`Cipher_Mode` interface requires message processing in
      * multiples of the block size. This returns size of required blocks to
      * update. If the mode implementation does not require buffering it will
      * return 1.
      * @return size of required blocks to update
      */
      virtual size_t update_granularity() const = 0;

      /**
      * Return an ideal granularity. This will be a multiple of the result of
      * update_granularity but may be larger. If so it indicates that better
      * performance may be achieved by providing buffers that are at least that
      * size (due to SIMD execution, etc).
      */
      virtual size_t ideal_granularity() const = 0;

      /**
      * Certain modes require the entire message be available before
      * any processing can occur. For such modes, input will be consumed
      * but not returned, until `finish` is called, which returns the
      * entire message.
      *
      * This function returns true if this mode has this style of
      * operation.
      */
      virtual bool requires_entire_message() const { return false; }

      /**
      * @return required minimum size to finalize() - may be any
      *         length larger than this.
      */
      virtual size_t minimum_final_size() const = 0;

      /**
      * @return the default size for a nonce
      */
      virtual size_t default_nonce_length() const = 0;

      /**
      * @return true iff nonce_len is a valid length for the nonce
      */
      virtual bool valid_nonce_length(size_t nonce_len) const = 0;

      /**
      * Resets just the message specific state and allows encrypting again under the existing key
      */
      virtual void reset() = 0;

      /**
      * Return the length in bytes of the authentication tag this algorithm
      * generates. If the mode is not authenticated, this will return 0.
      *
      * @return true iff this mode provides authentication as well as
      *         confidentiality.
      */
      bool authenticated() const { return this->tag_size() > 0; }

      /**
      * @return the size of the authentication tag used (in bytes)
      */
      virtual size_t tag_size() const { return 0; }

      /**
      * @return provider information about this implementation. Default is "base",
      * might also return "sse2", "avx2", "openssl", or some other arbitrary string.
      */
      virtual std::string provider() const { return "base"; }
};

/**
* Get a cipher mode by name (eg "AES-128/CBC" or "Serpent/XTS")
* @param algo_spec cipher name
* @param direction Cipher_Dir::Encryption or Cipher_Dir::Decryption
* @param provider provider implementation to choose
*/
BOTAN_DEPRECATED("Use Cipher_Mode::create")
inline Cipher_Mode* get_cipher_mode(std::string_view algo_spec, Cipher_Dir direction, std::string_view provider = "") {
   return Cipher_Mode::create(algo_spec, direction, provider).release();
}

}  // namespace Botan

// NOLINTBEGIN(*-macro-usage)

BOTAN_FUTURE_INTERNAL_HEADER(compiler.h)

/*
* Define BOTAN_COMPILER_HAS_BUILTIN
*/
#if defined(__has_builtin)
   #define BOTAN_COMPILER_HAS_BUILTIN(x) __has_builtin(x)
#else
   #define BOTAN_COMPILER_HAS_BUILTIN(x) 0
#endif

/*
* Define BOTAN_COMPILER_HAS_ATTRIBUTE
*/
#if defined(__has_attribute)
   #define BOTAN_COMPILER_HAS_ATTRIBUTE(x) __has_attribute(x)
   #define BOTAN_COMPILER_ATTRIBUTE(x) __attribute__((x))
#else
   #define BOTAN_COMPILER_HAS_ATTRIBUTE(x) 0
   #define BOTAN_COMPILER_ATTRIBUTE(x) /**/
#endif

/*
* Hack for Loongarch64 GCC bug
*
* For some reason __has_attribute(target) is true, but it does not support the
* target attribute... this supposedly is fixed in GCC 15 but this is untested.
*/
#if defined(__GNUC__) && defined(__loongarch64) && (__GNUC__ <= 14)
   #define BOTAN_COMPILER_DOES_NOT_HAVE_TARGET_ATTRIBUTE
#endif

/*
* Define BOTAN_FUNC_ISA
*
* TODO(Botan4) Move this to isa_extn.h
*/
#if BOTAN_COMPILER_HAS_ATTRIBUTE(target) && !defined(BOTAN_COMPILER_DOES_NOT_HAVE_TARGET_ATTRIBUTE)
   #define BOTAN_FUNC_ISA(isa) BOTAN_COMPILER_ATTRIBUTE(target(isa))
#else
   #define BOTAN_FUNC_ISA(isa)
#endif

/*
* Define BOTAN_FUNC_ISA_INLINE
*
* TODO(Botan4) Remove this
*/
#define BOTAN_FUNC_ISA_INLINE(isa) BOTAN_FUNC_ISA(isa) BOTAN_FORCE_INLINE

/*
* Define BOTAN_EARLY_INIT
*/
#if BOTAN_COMPILER_HAS_ATTRIBUTE(init_priority)
   #define BOTAN_EARLY_INIT(prio) BOTAN_COMPILER_ATTRIBUTE(init_priority(prio))
#else
   #define BOTAN_EARLY_INIT(prio) /**/
#endif

/*
* Define BOTAN_FORCE_INLINE
*/
#if !defined(BOTAN_FORCE_INLINE)

   #if BOTAN_COMPILER_HAS_ATTRIBUTE(always_inline)
      #define BOTAN_FORCE_INLINE inline BOTAN_COMPILER_ATTRIBUTE(always_inline)

   #elif defined(_MSC_VER)
      #define BOTAN_FORCE_INLINE __forceinline

   #else
      #define BOTAN_FORCE_INLINE inline
   #endif

#endif

// NOLINTEND(*-macro-usage)

namespace Botan {

/**
* This class represents an abstract data source object.
*/
class BOTAN_PUBLIC_API(2, 0) DataSource {
   public:
      /**
      * Read from the source. Moves the internal offset so that every
      * call to read will return a new portion of the source.
      *
      * @param out the byte array to write the result to
      * @param length the length of the byte array out
      * @return length in bytes that was actually read and put
      * into out
      */
      [[nodiscard]] virtual size_t read(uint8_t out[], size_t length) = 0;

      virtual bool check_available(size_t n) = 0;

      /**
      * Read from the source but do not modify the internal
      * offset. Consecutive calls to peek() will return portions of
      * the source starting at the same position.
      *
      * @param out the byte array to write the output to
      * @param length the length of the byte array out
      * @param peek_offset the offset into the stream to read at
      * @return length in bytes that was actually read and put
      * into out
      */
      [[nodiscard]] virtual size_t peek(uint8_t out[], size_t length, size_t peek_offset) const = 0;

      /**
      * Test whether the source still has data that can be read.
      * @return true if there is no more data to read, false otherwise
      */
      virtual bool end_of_data() const = 0;

      /**
      * return the id of this data source
      * @return std::string representing the id of this data source
      */
      virtual std::string id() const { return ""; }

      /**
      * Read one byte.
      * @param out the byte to read to
      * @return length in bytes that was actually read and put
      * into out
      */
      size_t read_byte(uint8_t& out);

      /**
      * Read one byte.
      *
      * Returns nullopt if no further bytes are available
      */
      std::optional<uint8_t> read_byte();

      /**
      * Peek at one byte.
      * @param out an output byte
      * @return length in bytes that was actually read and put
      * into out
      */
      size_t peek_byte(uint8_t& out) const;

      /**
      * Discard the next N bytes of the data
      * @param N the number of bytes to discard
      * @return number of bytes actually discarded
      */
      size_t discard_next(size_t N);

      /**
      * @return number of bytes read so far.
      */
      virtual size_t get_bytes_read() const = 0;

      DataSource() = default;
      virtual ~DataSource() = default;
      DataSource(const DataSource&) = delete;
      DataSource(DataSource&&) = default;
      DataSource& operator=(const DataSource&) = delete;
      DataSource& operator=(DataSource&&) = default;
};

/**
* This class represents a Memory-Based DataSource
*/
class BOTAN_PUBLIC_API(2, 0) DataSource_Memory final : public DataSource {
   public:
      size_t read(uint8_t buf[], size_t length) override;
      size_t peek(uint8_t buf[], size_t length, size_t offset) const override;
      bool check_available(size_t n) override;
      bool end_of_data() const override;

      /**
      * Construct a memory source that reads from a string
      * @param in the string to read from
      */
      explicit DataSource_Memory(std::string_view in);

      /**
      * Construct a memory source that reads from a byte array
      * @param in the byte array to read from
      * @param length the length of the byte array
      */
      DataSource_Memory(const uint8_t in[], size_t length) : m_source(in, in + length), m_offset(0) {}

      /**
      * Construct a memory source that reads from a secure_vector
      * @param in the MemoryRegion to read from
      */
      explicit DataSource_Memory(secure_vector<uint8_t> in) : m_source(std::move(in)), m_offset(0) {}

      /**
      * Construct a memory source that reads from an arbitrary byte buffer
      * @param in the MemoryRegion to read from
      */
      explicit DataSource_Memory(std::span<const uint8_t> in) : m_source(in.begin(), in.end()), m_offset(0) {}

      /**
      * Construct a memory source that reads from a std::vector
      * @param in the MemoryRegion to read from
      */
      explicit DataSource_Memory(const std::vector<uint8_t>& in) : m_source(in.begin(), in.end()), m_offset(0) {}

      size_t get_bytes_read() const override { return m_offset; }

   private:
      secure_vector<uint8_t> m_source;
      size_t m_offset;
};

/**
* This class represents a Stream-Based DataSource.
*/
class BOTAN_PUBLIC_API(2, 0) DataSource_Stream final : public DataSource {
   public:
      size_t read(uint8_t buf[], size_t length) override;
      size_t peek(uint8_t buf[], size_t length, size_t offset) const override;
      bool check_available(size_t n) override;
      bool end_of_data() const override;
      std::string id() const override;

      BOTAN_FUTURE_EXPLICIT DataSource_Stream(std::istream& in, std::string_view id = "<std::istream>");

#if defined(BOTAN_TARGET_OS_HAS_FILESYSTEM)
      /**
      * Construct a Stream-Based DataSource from filesystem path
      * @param filename the path to the file
      * @param use_binary whether to treat the file as binary or not
      */
      BOTAN_FUTURE_EXPLICIT DataSource_Stream(std::string_view filename, bool use_binary = false);
#endif

      DataSource_Stream(const DataSource_Stream&) = delete;
      DataSource_Stream(DataSource_Stream&&) = delete;
      DataSource_Stream& operator=(const DataSource_Stream&) = delete;
      DataSource_Stream& operator=(DataSource_Stream&&) = delete;

      ~DataSource_Stream() override;

      size_t get_bytes_read() const override { return m_total_read; }

   private:
      const std::string m_identifier;

      std::unique_ptr<std::istream> m_source_memory;
      std::istream& m_source;
      size_t m_total_read;
};

}  // namespace Botan

namespace Botan {

class BigInt;

/**
* General DER Encoding Object
*/
class BOTAN_PUBLIC_API(2, 0) DER_Encoder final {
   public:
      typedef std::function<void(const uint8_t[], size_t)> append_fn;

      /**
      * DER encode, writing to an internal buffer
      * Use get_contents or get_contents_unlocked to read the results
      * after all encoding is completed.
      */
      DER_Encoder() = default;

      /**
      * DER encode, writing to @param vec
      * If this constructor is used, get_contents* may not be called.
      */
      BOTAN_FUTURE_EXPLICIT DER_Encoder(secure_vector<uint8_t>& vec);

      /**
      * DER encode, writing to @param vec
      * If this constructor is used, get_contents* may not be called.
      */
      BOTAN_FUTURE_EXPLICIT DER_Encoder(std::vector<uint8_t>& vec);

      /**
      * DER encode, calling append to write output
      * If this constructor is used, get_contents* may not be called.
      */
      BOTAN_FUTURE_EXPLICIT DER_Encoder(append_fn append) : m_append_output(std::move(append)) {}

      secure_vector<uint8_t> get_contents();

      /**
      * Return the encoded contents as a std::vector
      *
      * If using this function, instead pass a std::vector to the
      * constructor of DER_Encoder where the output will be placed. This
      * avoids several unnecessary copies.
      */
      BOTAN_DEPRECATED("Use DER_Encoder(vector) instead") std::vector<uint8_t> get_contents_unlocked();

      DER_Encoder& start_cons(ASN1_Type type_tag, ASN1_Class class_tag);

      DER_Encoder& start_sequence() { return start_cons(ASN1_Type::Sequence, ASN1_Class::Universal); }

      DER_Encoder& start_set() { return start_cons(ASN1_Type::Set, ASN1_Class::Universal); }

      DER_Encoder& start_context_specific(uint32_t tag) {
         return start_cons(ASN1_Type(tag), ASN1_Class::ContextSpecific);
      }

      DER_Encoder& start_explicit_context_specific(uint32_t tag) {
         return start_cons(ASN1_Type(tag), ASN1_Class::ExplicitContextSpecific);
      }

      DER_Encoder& end_cons();

      DER_Encoder& start_explicit(uint16_t type_tag);
      DER_Encoder& end_explicit();

      /**
      * Insert raw bytes directly into the output stream
      */
      DER_Encoder& raw_bytes(const uint8_t val[], size_t len);

      DER_Encoder& raw_bytes(std::span<const uint8_t> val) { return raw_bytes(val.data(), val.size()); }

      DER_Encoder& encode_null();
      DER_Encoder& encode(bool b);
      DER_Encoder& encode(size_t s);
      DER_Encoder& encode(const BigInt& n);
      DER_Encoder& encode(const uint8_t val[], size_t len, ASN1_Type real_type);

      template <typename Alloc>
      DER_Encoder& encode(const std::vector<uint8_t, Alloc>& vec, ASN1_Type real_type) {
         return encode(vec.data(), vec.size(), real_type);
      }

      DER_Encoder& encode(bool b, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      DER_Encoder& encode(size_t s, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      DER_Encoder& encode(const BigInt& n, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      DER_Encoder& encode(const uint8_t v[],
                          size_t len,
                          ASN1_Type real_type,
                          ASN1_Type type_tag,
                          ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      template <typename Alloc>
      DER_Encoder& encode(const std::vector<uint8_t, Alloc>& bytes,
                          ASN1_Type real_type,
                          ASN1_Type type_tag,
                          ASN1_Class class_tag) {
         return encode(bytes.data(), bytes.size(), real_type, type_tag, class_tag);
      }

      template <typename T>
      BOTAN_DEPRECATED("Use the version that takes a std::optional")
      DER_Encoder& encode_optional(const T& value, const T& default_value) {
         if(value != default_value) {
            encode(value);
         }
         return (*this);
      }

      template <typename T>
      DER_Encoder& encode_optional(const std::optional<T>& value) {
         if(value) {
            encode(*value);
         }
         return (*this);
      }

      template <typename T>
      DER_Encoder& encode_list(const std::vector<T>& values) {
         for(size_t i = 0; i != values.size(); ++i) {
            encode(values[i]);
         }
         return (*this);
      }

      /*
      * Request for an object to encode itself to this stream
      */
      DER_Encoder& encode(const ASN1_Object& obj);

      /*
      * Conditionally write some values to the stream
      */
      DER_Encoder& encode_if(bool pred, DER_Encoder& enc) {
         if(pred) {
            return raw_bytes(enc.get_contents());
         }
         return (*this);
      }

      DER_Encoder& encode_if(bool pred, const ASN1_Object& obj) {
         if(pred) {
            encode(obj);
         }
         return (*this);
      }

      DER_Encoder& encode_if(bool pred, size_t num) {
         if(pred) {
            encode(num);
         }
         return (*this);
      }

      DER_Encoder& encode_if(bool pred, bool num) {
         if(pred) {
            encode(num);
         }
         return (*this);
      }

      DER_Encoder& add_object(ASN1_Type type_tag, ASN1_Class class_tag, const uint8_t rep[], size_t length);

      DER_Encoder& add_object(ASN1_Type type_tag, ASN1_Class class_tag, std::span<const uint8_t> rep) {
         return add_object(type_tag, class_tag, rep.data(), rep.size());
      }

      DER_Encoder& add_object(ASN1_Type type_tag, ASN1_Class class_tag, const std::vector<uint8_t>& rep) {
         return add_object(type_tag, class_tag, std::span{rep});
      }

      DER_Encoder& add_object(ASN1_Type type_tag, ASN1_Class class_tag, const secure_vector<uint8_t>& rep) {
         return add_object(type_tag, class_tag, std::span{rep});
      }

      DER_Encoder& add_object(ASN1_Type type_tag, ASN1_Class class_tag, std::string_view str);

      DER_Encoder& add_object(ASN1_Type type_tag, ASN1_Class class_tag, uint8_t val);

   private:
      class DER_Sequence final {
         public:
            uint32_t tag_of() const;

            void push_contents(DER_Encoder& der);

            void add_bytes(const uint8_t val[], size_t len);

            void add_bytes(const uint8_t hdr[], size_t hdr_len, const uint8_t val[], size_t val_len);

            DER_Sequence(ASN1_Type type_tag, ASN1_Class class_tag);

            DER_Sequence(DER_Sequence&& seq) noexcept :
                  m_type_tag(seq.m_type_tag),
                  m_class_tag(seq.m_class_tag),
                  m_contents(std::move(seq.m_contents)),
                  m_set_contents(std::move(seq.m_set_contents)) {}

            DER_Sequence& operator=(DER_Sequence&& seq) noexcept {
               std::swap(m_type_tag, seq.m_type_tag);
               std::swap(m_class_tag, seq.m_class_tag);
               std::swap(m_contents, seq.m_contents);
               std::swap(m_set_contents, seq.m_set_contents);
               return (*this);
            }

            DER_Sequence(const DER_Sequence& seq) = default;
            DER_Sequence& operator=(const DER_Sequence& seq) = default;
            ~DER_Sequence() = default;

         private:
            ASN1_Type m_type_tag;
            ASN1_Class m_class_tag;
            secure_vector<uint8_t> m_contents;
            std::vector<secure_vector<uint8_t>> m_set_contents;
      };

      append_fn m_append_output;
      secure_vector<uint8_t> m_default_outbuf;
      std::vector<DER_Sequence> m_subsequences;
};

}  // namespace Botan

/*
* This header is public, but avoid including it directly. Instead
* get the contents via ec_group.h.
*/

namespace Botan {

/**
* This enum indicates the method used to encode the EC parameters
*
* @warning All support for explicit or implicit domain encodings
* will be removed in Botan4. Only named curves will be supported.
*
* TODO(Botan4) remove this enum
*/
enum class EC_Group_Encoding : uint8_t {
   Explicit = 0,
   ImplicitCA = 1,
   NamedCurve = 2,

   EC_DOMPAR_ENC_EXPLICIT = Explicit,
   EC_DOMPAR_ENC_IMPLICITCA = ImplicitCA,
   EC_DOMPAR_ENC_OID = NamedCurve
};

enum class EC_Point_Format : uint8_t {
   Uncompressed = 0,
   Compressed = 1,

   UNCOMPRESSED BOTAN_DEPRECATED("Use EC_Point_Format::Uncompressed") = Uncompressed,
   COMPRESSED BOTAN_DEPRECATED("Use EC_Point_Format::Compressed") = Compressed,

   Hybrid BOTAN_DEPRECATED("Hybrid point encoding is deprecated") = 2,
   HYBRID BOTAN_DEPRECATED("Hybrid point encoding is deprecated") = 2
};

}  // namespace Botan

namespace Botan {

class BigInt;
class RandomNumberGenerator;
class EC_Group;
class EC_Scalar;

#if defined(BOTAN_HAS_LEGACY_EC_POINT)
class EC_Point;
#endif

class EC_Group_Data;
class EC_AffinePoint_Data;

/// Elliptic Curve Point in Affine Representation
///
class BOTAN_PUBLIC_API(3, 6) EC_AffinePoint final {
   public:
      /// Point deserialization. Throws if wrong length or not a valid point
      ///
      /// This accepts SEC1 compressed or uncompressed formats
      EC_AffinePoint(const EC_Group& group, std::span<const uint8_t> bytes);

      /// Point deserialization. Returns nullopt if wrong length or not a valid point
      ///
      /// This accepts SEC1 compressed or uncompressed formats
      static std::optional<EC_AffinePoint> deserialize(const EC_Group& group, std::span<const uint8_t> bytes);

      /// Create a point from a pair (x,y) of integers
      ///
      /// The integers must be within the field - in the range [0,p) and must
      /// satisfy the curve equation
      static std::optional<EC_AffinePoint> from_bigint_xy(const EC_Group& group, const BigInt& x, const BigInt& y);

      /// Multiply by the group generator returning a complete point
      static EC_AffinePoint g_mul(const EC_Scalar& scalar, RandomNumberGenerator& rng);

      /// Return the identity element
      static EC_AffinePoint identity(const EC_Group& group);

      /// Return the standard group generator
      static EC_AffinePoint generator(const EC_Group& group);

      /// Hash to curve (RFC 9380), random oracle variant
      ///
      /// Only supported for specific groups
      static EC_AffinePoint hash_to_curve_ro(const EC_Group& group,
                                             std::string_view hash_fn,
                                             std::span<const uint8_t> input,
                                             std::span<const uint8_t> domain_sep);

      /// Hash to curve (RFC 9380), random oracle variant
      ///
      /// Only supported for specific groups
      static EC_AffinePoint hash_to_curve_ro(const EC_Group& group,
                                             std::string_view hash_fn,
                                             std::span<const uint8_t> input,
                                             std::string_view domain_sep);

      /// Hash to curve (RFC 9380), non uniform variant
      ///
      /// Only supported for specific groups
      static EC_AffinePoint hash_to_curve_nu(const EC_Group& group,
                                             std::string_view hash_fn,
                                             std::span<const uint8_t> input,
                                             std::span<const uint8_t> domain_sep);

      /// Hash to curve (RFC 9380), non uniform variant
      ///
      /// Only supported for specific groups
      static EC_AffinePoint hash_to_curve_nu(const EC_Group& group,
                                             std::string_view hash_fn,
                                             std::span<const uint8_t> input,
                                             std::string_view domain_sep);

      /// Multiply a point by a scalar returning a complete point
      EC_AffinePoint mul(const EC_Scalar& scalar, RandomNumberGenerator& rng) const;

      /// Multiply a point by a scalar, returning the byte encoding of the x coordinate only
      secure_vector<uint8_t> mul_x_only(const EC_Scalar& scalar, RandomNumberGenerator& rng) const;

      /// Compute 2-ary multiscalar multiplication - p*x + q*y
      ///
      /// This operation runs in constant time with respect to p, x, q, and y
      ///
      /// @returns p*x+q*y, or nullopt if the result was the point at infinity
      static std::optional<EC_AffinePoint> mul_px_qy(const EC_AffinePoint& p,
                                                     const EC_Scalar& x,
                                                     const EC_AffinePoint& q,
                                                     const EC_Scalar& y,
                                                     RandomNumberGenerator& rng);

      /// Point addition
      ///
      /// Note that this is quite slow since it converts the resulting
      /// projective point immediately to affine coordinates, which requires a
      /// field inversion. This can be sufficient when implementing protocols
      /// that just need to perform a few additions.
      ///
      /// In the future a corresponding EC_ProjectivePoint type may be added
      /// which would avoid the expensive affine conversions
      EC_AffinePoint add(const EC_AffinePoint& q) const;

      /// Point negation
      EC_AffinePoint negate() const;

      /// Return the number of bytes of a field element
      ///
      /// A point consists of two field elements, plus possibly a header
      size_t field_element_bytes() const;

      /// Return true if this point is the identity element
      bool is_identity() const;

      /// Write the fixed length encoding of affine x coordinate
      ///
      /// The output span must be exactly field_element_bytes long
      ///
      /// This function will fail if this point is the identity element
      void serialize_x_to(std::span<uint8_t> bytes) const;

      /// Write the fixed length encoding of affine y coordinate
      ///
      /// The output span must be exactly field_element_bytes long
      ///
      /// This function will fail if this point is the identity element
      void serialize_y_to(std::span<uint8_t> bytes) const;

      /// Write the fixed length encoding of affine x and y coordinates
      ///
      /// The output span must be exactly 2*field_element_bytes long
      ///
      /// This function will fail if this point is the identity element
      void serialize_xy_to(std::span<uint8_t> bytes) const;

      /// Write the fixed length SEC1 compressed encoding
      ///
      /// The output span must be exactly 1 + field_element_bytes long
      ///
      /// This function will fail if this point is the identity element
      void serialize_compressed_to(std::span<uint8_t> bytes) const;

      /// Return the fixed length encoding of SEC1 uncompressed encoding
      ///
      /// The output span must be exactly 1 + 2*field_element_bytes long
      ///
      /// This function will fail if this point is the identity element
      void serialize_uncompressed_to(std::span<uint8_t> bytes) const;

      /// Return the bytes of the affine x coordinate in a container
      ///
      /// This function will fail if this point is the identity element
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T x_bytes() const {
         T bytes(this->field_element_bytes());
         this->serialize_x_to(bytes);
         return bytes;
      }

      /// Return the bytes of the affine y coordinate in a container
      ///
      /// This function will fail if this point is the identity element
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T y_bytes() const {
         T bytes(this->field_element_bytes());
         this->serialize_y_to(bytes);
         return bytes;
      }

      /// Return the bytes of the affine x and y coordinates in a container
      ///
      /// This function will fail if this point is the identity element
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T xy_bytes() const {
         T bytes(2 * this->field_element_bytes());
         this->serialize_xy_to(bytes);
         return bytes;
      }

      /// Return the bytes of the affine x and y coordinates in a container
      ///
      /// This function will fail if this point is the identity element
      template <concepts::resizable_byte_buffer T = std::vector<uint8_t>>
      T serialize_uncompressed() const {
         T bytes(1 + 2 * this->field_element_bytes());
         this->serialize_uncompressed_to(bytes);
         return bytes;
      }

      /// Return the bytes of the affine x and y coordinates in a container
      ///
      /// This function will fail if this point is the identity element
      template <concepts::resizable_byte_buffer T = std::vector<uint8_t>>
      T serialize_compressed() const {
         T bytes(1 + this->field_element_bytes());
         this->serialize_compressed_to(bytes);
         return bytes;
      }

      bool operator==(const EC_AffinePoint& other) const;

      bool operator!=(const EC_AffinePoint& other) const { return !(*this == other); }

      /// Return an encoding depending on the requested format
      std::vector<uint8_t> serialize(EC_Point_Format format) const;

      EC_AffinePoint(const EC_AffinePoint& other);
      EC_AffinePoint(EC_AffinePoint&& other) noexcept;

      EC_AffinePoint& operator=(const EC_AffinePoint& other);
      EC_AffinePoint& operator=(EC_AffinePoint&& other) noexcept;

#if defined(BOTAN_HAS_LEGACY_EC_POINT)
      /**
      * Deprecated conversion
      */
      EC_AffinePoint(const EC_Group& group, const EC_Point& pt);

      /**
      * Deprecated conversion
      */
      EC_Point to_legacy_point() const;
#endif

      BOTAN_DEPRECATED("Use version without workspace arg")
      static EC_AffinePoint g_mul(const EC_Scalar& scalar, RandomNumberGenerator& rng, std::vector<BigInt>& /*ws*/) {
         return EC_AffinePoint::g_mul(scalar, rng);
      }

      BOTAN_DEPRECATED("Use version without workspace arg")
      EC_AffinePoint mul(const EC_Scalar& scalar, RandomNumberGenerator& rng, std::vector<BigInt>& /*ws*/) const {
         return this->mul(scalar, rng);
      }

      /// Multiply a point by a scalar, returning the byte encoding of the x coordinate only
      secure_vector<uint8_t> mul_x_only(const EC_Scalar& scalar,
                                        RandomNumberGenerator& rng,
                                        std::vector<BigInt>& /*ws*/) const {
         return this->mul_x_only(scalar, rng);
      }

      ~EC_AffinePoint();

      const EC_AffinePoint_Data& _inner() const { return inner(); }

      static EC_AffinePoint _from_inner(std::unique_ptr<EC_AffinePoint_Data> inner);

      const std::shared_ptr<const EC_Group_Data>& _group() const;

   private:
      friend class EC_Mul2Table;

      explicit EC_AffinePoint(std::unique_ptr<EC_AffinePoint_Data> point);

      const EC_AffinePoint_Data& inner() const { return *m_point; }

      std::unique_ptr<EC_AffinePoint_Data> m_point;
};

}  // namespace Botan

namespace Botan {

class BigInt;
class RandomNumberGenerator;
class EC_Group;
class EC_Scalar_Data;

/**
* Represents an integer modulo the prime group order of an elliptic curve
*/
class BOTAN_PUBLIC_API(3, 6) EC_Scalar final {
   public:
      /**
      * Deserialize a scalar
      *
      * The span must be exactly bytes() long; this function does not accept
      * either short inputs (eg [1] to encode the integer 1) or inputs with
      * excess leading zero bytes.
      *
      * Returns nullopt if the length is incorrect or if the integer is not
      * within the range [0,n) where n is the group order.
      */
      static std::optional<EC_Scalar> deserialize(const EC_Group& group, std::span<const uint8_t> bytes);

      /**
      * Convert a bytestring to an EC_Scalar
      *
      * This uses the truncation rules from ECDSA
      */
      static EC_Scalar from_bytes_with_trunc(const EC_Group& group, std::span<const uint8_t> bytes);

      /**
      * Convert a bytestring to an EC_Scalar
      *
      * This reduces the bytes modulo the group order. The input can be at most
      * 2*bytes() long
      */
      static EC_Scalar from_bytes_mod_order(const EC_Group& group, std::span<const uint8_t> bytes);

      /**
      * Hash to scalar following RFC 9380
      *
      * This requires XMD. Unlike hash2curve, any group is supported
      */
      static EC_Scalar hash(const EC_Group& group,
                            std::string_view hash_fn,
                            std::span<const uint8_t> input,
                            std::span<const uint8_t> domain_sep);

      /**
      * Convert a bytestring to an EC_Scalar
      *
      * This is similar to deserialize but instead of returning nullopt if the input
      * is invalid, it will throw an exception.
      */
      BOTAN_DEPRECATED("Use EC_Scalar::deserialize") EC_Scalar(const EC_Group& group, std::span<const uint8_t> bytes);

      /**
      * Deserialize a pair of scalars
      *
      * Returns nullopt if the length is not 2*bytes(), or if either scalar is
      * out of range or zero
      */
      static std::optional<std::pair<EC_Scalar, EC_Scalar>> deserialize_pair(const EC_Group& group,
                                                                             std::span<const uint8_t> bytes);

      /**
      * Return a new random scalar value
      */
      static EC_Scalar random(const EC_Group& group, RandomNumberGenerator& rng);

      /**
      * Return the scalar value 1
      */
      static EC_Scalar one(const EC_Group& group);

      /**
      * Convert from the argument BigInt to a EC_Scalar
      *
      * Throws an exception if the provided bn is negative or too large
      */
      static EC_Scalar from_bigint(const EC_Group& group, const BigInt& bn);

      /**
      * Compute the elliptic curve scalar multiplication (g*k) where g is the
      * standard base point on the curve. Then extract the x coordinate of
      * the resulting point, and reduce it modulo the group order.
      */
      static EC_Scalar gk_x_mod_order(const EC_Scalar& scalar, RandomNumberGenerator& rng);

      BOTAN_DEPRECATED("Use version without workspace arg")
      static EC_Scalar
         gk_x_mod_order(const EC_Scalar& scalar, RandomNumberGenerator& rng, std::vector<BigInt>& /*ws*/) {
         return EC_Scalar::gk_x_mod_order(scalar, rng);
      }

      /**
      * Return the byte size of this scalar
      */
      size_t bytes() const;

      /**
      * Write the fixed length serialization to bytes
      *
      * The provided span must be exactly bytes() long
      */
      void serialize_to(std::span<uint8_t> bytes) const;

      /**
      * Return the bytes of the encoded scalar in a container
      */
      template <concepts::resizable_byte_buffer T = std::vector<uint8_t>>
      T serialize() const {
         T s(this->bytes());
         this->serialize_to(s);
         return s;
      }

      /**
      * Write the fixed length serialization to bytes
      *
      * The provided span must be exactly 2*bytes() long
      */
      static void serialize_pair_to(std::span<uint8_t> bytes, const EC_Scalar& r, const EC_Scalar& s);

      /**
      * Return the bytes of the encoded scalar in a container
      */
      template <concepts::resizable_byte_buffer T = std::vector<uint8_t>>
      static T serialize_pair(const EC_Scalar& r, const EC_Scalar& s) {
         T bytes(r.bytes() + s.bytes());
         serialize_pair_to(bytes, r, s);
         return bytes;
      }

      /**
      * Return true if this EC_Scalar is zero
      */
      bool is_zero() const;

      /**
      * Return true if this EC_Scalar is not zero
      */
      bool is_nonzero() const { return !is_zero(); }

      /**
      * Constant time modular inversion
      *
      * Return the modular inverse of this EC_Scalar
      *
      * If *this is zero, then invert() returns zero
      */
      EC_Scalar invert() const;

      /**
      * Variable time modular inversion
      *
      * Return the modular inverse of this EC_Scalar
      *
      * If *this is zero, then invert_vartime() returns zero
      */
      EC_Scalar invert_vartime() const;

      /**
      * Return the additive inverse of *this
      */
      EC_Scalar negate() const;

      /**
      * Scalar addition (modulo group order)
      */
      EC_Scalar add(const EC_Scalar& x) const;

      /**
      * Scalar subtraction (modulo group order)
      */
      EC_Scalar sub(const EC_Scalar& x) const;

      /**
      * Scalar multiplication (modulo group order)
      */
      EC_Scalar mul(const EC_Scalar& x) const;

      /**
      * Assign a scalar
      */
      void assign(const EC_Scalar& x);

      /**
      * Equivalent to assigning a zero value, but also does so in a way that
      * attempts to ensure the write always occurs even if a compiler can deduce
      * the assignment is otherwise unnecessary.
      */
      void zeroize();

      /**
      * Set *this to its own square modulo the group order
      */
      void square_self();

      /**
      * Test for equality
      */
      bool is_eq(const EC_Scalar& x) const;

      /**
      * Convert *this to a BigInt
      */
      BigInt to_bigint() const;

      friend EC_Scalar operator+(const EC_Scalar& x, const EC_Scalar& y) { return x.add(y); }

      friend EC_Scalar operator-(const EC_Scalar& x, const EC_Scalar& y) { return x.sub(y); }

      friend EC_Scalar operator*(const EC_Scalar& x, const EC_Scalar& y) { return x.mul(y); }

      friend bool operator==(const EC_Scalar& x, const EC_Scalar& y) { return x.is_eq(y); }

      EC_Scalar(const EC_Scalar& other);
      EC_Scalar(EC_Scalar&& other) noexcept;

      EC_Scalar& operator=(const EC_Scalar& other);
      EC_Scalar& operator=(EC_Scalar&& other) noexcept;

      ~EC_Scalar();

      const EC_Scalar_Data& _inner() const { return inner(); }

      static EC_Scalar _from_inner(std::unique_ptr<EC_Scalar_Data> inner);

   private:
      friend class EC_AffinePoint;

      explicit EC_Scalar(std::unique_ptr<EC_Scalar_Data> scalar);

      const EC_Scalar_Data& inner() const { return *m_scalar; }

      std::unique_ptr<EC_Scalar_Data> m_scalar;
};

}  // namespace Botan

#if defined(BOTAN_HAS_LEGACY_EC_POINT)
#endif

namespace Botan {

/**
* This enum indicates the source of the elliptic curve parameters
* in use.
*
* Builtin means the curve is a known standard one which was compiled
* in the library.
*
* ExternalSource means the curve parameters came from either an explicit
* curve encoding or an application defined curve.
*/
enum class EC_Group_Source : uint8_t {
   Builtin,
   ExternalSource,
};

/**
* Enum indicating the way the group in question is implemented
*
* This is returned by EC_Group::engine
*/
enum class EC_Group_Engine : uint8_t {
   /// Using per curve implementation; fastest available
   Optimized,
   /// A generic implementation that handles many curves in one implementation
   Generic,
   /// The old implementation, used as a fallback if none of the other
   /// implementations can be used
   /// TODO(Botan4) remove this
   Legacy,
};

class EC_Mul2Table_Data;
class EC_Group_Data;
class EC_Group_Data_Map;

/**
* Class representing an elliptic curve
*
* The internal representation is stored in a shared_ptr, so copying an
* EC_Group is inexpensive.
*/
class BOTAN_PUBLIC_API(2, 0) EC_Group final {
   public:
      /**
      * Construct elliptic curve from the specified parameters
      *
      * This is used for example to create custom (application-specific) curves.
      *
      * Some build configurations do not support application specific curves, in
      * which case this constructor will throw an exception. You can check for
      * this situation beforehand using the function
      * EC_Group::supports_application_specific_group()
      *
      * @param p the elliptic curve p
      * @param a the elliptic curve a param
      * @param b the elliptic curve b param
      * @param base_x the x coordinate of the base point
      * @param base_y the y coordinate of the base point
      * @param order the order of the base point
      * @param cofactor the cofactor
      * @param oid an optional OID used to identify this curve
      *
      * @warning This constructor is deprecated and will be removed in Botan 4
      *
      * @warning support for cofactors > 1 is deprecated and will be removed
      *
      * @warning support for prime fields > 521 bits is deprecated and
      * will be removed.
      *
      * @warning Support for explicitly encoded curve parameters is deprecated.
      * An OID must be assigned.
      */
      BOTAN_DEPRECATED("Use alternate constructor")
      EC_Group(const BigInt& p,
               const BigInt& a,
               const BigInt& b,
               const BigInt& base_x,
               const BigInt& base_y,
               const BigInt& order,
               const BigInt& cofactor,
               const OID& oid = OID());

      /**
      * Construct elliptic curve from the specified parameters
      *
      * This is used for example to create custom (application-specific) curves.
      *
      * Some build configurations do not support application specific curves, in
      * which case this constructor will throw an exception. You can check for
      * this situation beforehand using the function
      * EC_Group::supports_application_specific_group()
      *
      * Unlike the deprecated constructor, this constructor imposes additional
      * restrictions on the parameters, namely:
      *
      *  - An object identifier must be provided
      *
      *  - The prime must be at least 192 bits and at most 512 bits, and a multiple
      *    of 32 bits. Currently, as long as BOTAN_DISABLE_DEPRECATED_FEATURES is not
      *    set, this constructor accepts primes as small as 128 bits - this lower
      *    bound will be removed in the next major release.
      *
      *  - As an extension of the above restriction, the prime can also be exactly
      *    the 521-bit Mersenne prime (2**521-1) or exactly the 239-bit prime used in
      *    X9.62 239 bit groups (2**239 - 2**143 - 2**95 + 2**47 - 1)
      *
      *  - The prime must be congruent to 3 modulo 4
      *
      *  - The group order must have the same bit length as the prime. It is allowed
      *    for the order to be larger than p, but they must have the same bit length.
      *
      *  - Only prime order curves (with cofactor == 1) are allowed
      *
      * @warning use only elliptic curve parameters that you trust
      *
      * @param oid an object identifier used to identify this curve
      * @param p the elliptic curve prime (at most 521 bits)
      * @param a the elliptic curve a param
      * @param b the elliptic curve b param
      * @param base_x the x coordinate of the group generator
      * @param base_y the y coordinate of the group generator
      * @param order the order of the group
      */
      EC_Group(const OID& oid,
               const BigInt& p,
               const BigInt& a,
               const BigInt& b,
               const BigInt& base_x,
               const BigInt& base_y,
               const BigInt& order);

      /**
      * Decode a DER encoded ECC domain parameter set
      * @param der the bytes of the DER encoding
      */
      explicit EC_Group(std::span<const uint8_t> der);

      BOTAN_DEPRECATED("Use EC_Group(std::span)")
      EC_Group(const uint8_t der[], size_t der_len) : EC_Group(std::span{der, der_len}) {}

      /**
      * Create an EC domain by OID (or throw if unknown)
      * @param oid the OID of the EC domain to create
      */
      BOTAN_DEPRECATED("Use EC_Group::from_OID") explicit EC_Group(const OID& oid) { *this = EC_Group::from_OID(oid); }

      /**
      * Create an EC domain from PEM encoding (as from PEM_encode()), or
      * from an OID name (eg "secp256r1", or "1.2.840.10045.3.1.7")
      * @param pem_or_oid PEM-encoded data, or an OID
      *
      * @warning Support for PEM in this function is deprecated. Use
      * EC_Group::from_PEM or EC_Group::from_OID or EC_Group::from_name
      */
      BOTAN_DEPRECATED("Use EC_Group::from_{name,OID,PEM}") explicit EC_Group(std::string_view pem_or_oid);

      /**
      * Initialize an EC group from the PEM/ASN.1 encoding
      */
      static EC_Group from_PEM(std::string_view pem);

      /**
      * Initialize an EC group from a group named by an object identifier
      */
      static EC_Group from_OID(const OID& oid);

      /**
      * Initialize an EC group from a group common name (eg "secp256r1")
      */
      static EC_Group from_name(std::string_view name);

      BOTAN_DEPRECATED("Use EC_Group::from_PEM") static EC_Group EC_Group_from_PEM(std::string_view pem) {
         return EC_Group::from_PEM(pem);
      }

      /**
      * Create an uninitialized EC_Group
      */
      BOTAN_DEPRECATED("Deprecated no replacement") EC_Group();

      /**
      * Unregister a previously registered group.
      *
      * Using this is discouraged for normal use. This is only useful or necessary if
      * you are registering a very large number of distinct groups, and need to worry about memory constraints.
      *
      * Returns true if the group was found and unregistered.
      */
      static bool unregister(const OID& oid);

      ~EC_Group();

      EC_Group(const EC_Group&);
      EC_Group(EC_Group&&) = default;

      EC_Group& operator=(const EC_Group&);
      EC_Group& operator=(EC_Group&&) = default;

      bool initialized() const { return (m_data != nullptr); }

      /**
       * Verify EC_Group domain
       * @returns true if group is valid. false otherwise
       */
      bool verify_group(RandomNumberGenerator& rng, bool strong = false) const;

      bool operator==(const EC_Group& other) const;

      EC_Group_Source source() const;

      /**
      * Return true if in this build configuration it is possible to
      * register an application specific elliptic curve.
      */
      static bool supports_application_specific_group();

      /**
      * Return true if in this build configuration it is possible to
      * register an application specific elliptic curve with a cofactor
      * larger than 1.
      */
      static bool supports_application_specific_group_with_cofactor();

      /**
      * Return true if EC_Group::from_name(name) should succeed for this name
      * either because it is a group compiled into the library or it is a group
      * which has already been registered by the application at runtime.
      */
      static bool supports_named_group(std::string_view name);

      /**
      * Return true if this EC_Group was derived from an explicit encoding
      *
      * Explicit encoding of groups is deprecated; when support for explicit curves
      * is removed in a future major release, this function will also be removed.
      */
      bool used_explicit_encoding() const { return m_explicit_encoding; }

      /**
      * Return how this EC_Group is implemented under the hood
      *
      * This is mostly useful for diagnostic or debugging purposes
      */
      EC_Group_Engine engine() const;

      /**
      * Return a set of known named EC groups
      *
      * This returns a set of groups for which from_name should succeed.
      *
      * Note that the set of included groups can vary based on the build
      * configuration, and that this list does not include any groups registered
      * by the application at runtime.
      */
      static const std::set<std::string>& known_named_groups();

      /**
      * Create the DER encoding of this domain
      * @param form of encoding to use
      * @returns the group information encoded as DER
      */
      BOTAN_DEPRECATED("Use the variant that does not take EC_Group_Encoding")
      std::vector<uint8_t> DER_encode(EC_Group_Encoding form) const;

      /**
      * Create the DER encoding of this domain, using namedCurve format
      * @returns the group information encoded as DER
      */
      std::vector<uint8_t> DER_encode() const;

      /**
      * Return the PEM encoding
      * @return string containing PEM data
      *
      * @warning In Botan4 the form parameter will be removed and only
      * namedCurve will be supported
      *
      * TODO(Botan4) remove the argument
      */
      std::string PEM_encode(EC_Group_Encoding form = EC_Group_Encoding::Explicit) const;

      /**
      * Return the size of p in bits (same as get_p().bits())
      */
      size_t get_p_bits() const;

      /**
      * Return the size of p in bytes (same as get_p().bytes())
      */
      size_t get_p_bytes() const;

      /**
      * Return the size of group order in bits (same as get_order().bits())
      */
      size_t get_order_bits() const;

      /**
      * Return the size of the group order in bytes (same as get_order().bytes())
      */
      size_t get_order_bytes() const;

      /// Table for computing g*x + h*y
      class BOTAN_PUBLIC_API(3, 6) Mul2Table final {
         public:
            /**
            * Create a table for computing g*x + h*y
            */
            BOTAN_FUTURE_EXPLICIT Mul2Table(const EC_AffinePoint& h);

            /**
            * Return the elliptic curve point g*x + h*y
            *
            * Where g is the group generator and h is the value passed to the constructor
            *
            * Returns nullopt if g*x + h*y was the point at infinity
            *
            * @warning this function is variable time with respect to x and y
            */
            std::optional<EC_AffinePoint> mul2_vartime(const EC_Scalar& x, const EC_Scalar& y) const;

            /**
            * Check if v equals the x coordinate of g*x + h*y reduced modulo the order
            *
            * Where g is the group generator and h is the value passed to the constructor
            *
            * Returns false if unequal, including if g*x + h*y was the point at infinity
            *
            * @warning this function is variable time with respect to x and y
            */
            bool mul2_vartime_x_mod_order_eq(const EC_Scalar& v, const EC_Scalar& x, const EC_Scalar& y) const;

            /**
            * Check if v equals the x coordinate of g*x*c + h*y*c reduced modulo the order
            *
            * Where g is the group generator and h is the value passed to the constructor
            *
            * Returns false if unequal, including if g*x*c + h*y*c was the point at infinity
            *
            * @warning this function is variable time with respect to x and y
            */
            bool mul2_vartime_x_mod_order_eq(const EC_Scalar& v,
                                             const EC_Scalar& c,
                                             const EC_Scalar& x,
                                             const EC_Scalar& y) const;

            ~Mul2Table();
            Mul2Table(const Mul2Table& other) = delete;
            Mul2Table(Mul2Table&& other) noexcept;
            Mul2Table& operator=(const Mul2Table& other) = delete;
            Mul2Table& operator=(Mul2Table&& other) noexcept;

         private:
            std::unique_ptr<EC_Mul2Table_Data> m_tbl;
      };

      /**
      * Return the OID of these domain parameters
      * @result the OID
      */
      const OID& get_curve_oid() const;

      /**
      * Return the prime modulus of the field
      */
      const BigInt& get_p() const;

      /**
      * Return the a parameter of the elliptic curve equation
      */
      const BigInt& get_a() const;

      /**
      * Return the b parameter of the elliptic curve equation
      */
      const BigInt& get_b() const;

      /**
      * Return the x coordinate of the base point
      */
      const BigInt& get_g_x() const;

      /**
      * Return the y coordinate of the base point
      */
      const BigInt& get_g_y() const;

      /**
      * Return the order of the base point
      * @result order of the base point
      */
      const BigInt& get_order() const;

      /**
      * Return the cofactor
      * @result the cofactor
      * TODO(Botan4): Remove this
      */
      const BigInt& get_cofactor() const;

      /**
      * Return true if the cofactor is > 1
      * TODO(Botan4): Remove this
      */
      bool has_cofactor() const;

      /*
      * For internal use only
      * TODO(Botan4): Move this to an internal header
      */
      static std::shared_ptr<EC_Group_Data> EC_group_info(const OID& oid);

      /*
      * For internal use only
      *
      * @warning this invalidates pointers and can cause memory corruption.
      * This function exists only to be called in tests.
      *
      * TODO(Botan4): Move this to an internal header
      */
      static size_t clear_registered_curve_data();

      /*
      * For internal use only
      * TODO(Botan4): Move this to an internal header
      */
      static OID EC_group_identity_from_order(const BigInt& order);

      /*
      * For internal use only
      */
      const std::shared_ptr<EC_Group_Data>& _data() const { return m_data; }

#if defined(BOTAN_HAS_LEGACY_EC_POINT)
      /**
      * Check if y is a plausible point on the curve
      *
      * In particular, checks that it is a point on the curve, not infinity,
      * and that it has order matching the group.
      */
      bool verify_public_element(const EC_Point& y) const;

      /**
      * OS2ECP (Octet String To Elliptic Curve Point)
      *
      * Deserialize an encoded point. Verifies that the point is on the curve.
      */
      BOTAN_DEPRECATED("Use EC_AffinePoint::deserialize") EC_Point OS2ECP(const uint8_t bits[], size_t len) const {
         return EC_AffinePoint(*this, std::span{bits, len}).to_legacy_point();
      }

      BOTAN_DEPRECATED("Use EC_AffinePoint::deserialize")
      EC_Point OS2ECP(std::span<const uint8_t> encoded_point) const {
         return EC_AffinePoint(*this, encoded_point).to_legacy_point();
      }

      /**
      * Return group base point
      * @result base point
      */
      BOTAN_DEPRECATED("Use EC_AffinePoint::generator") const EC_Point& get_base_point() const;

      // Everything below here will be removed in a future release:

      /**
      * Return the canonical group generator
      * @result standard generator of the curve
      */
      BOTAN_DEPRECATED("Use EC_AffinePoint::generator") const EC_Point& generator() const;

      /**
      * Multi exponentiate. Not constant time.
      * @return base_point*x + h*y
      */
      BOTAN_DEPRECATED("Use EC_Group::Mul2Table")
      EC_Point point_multiply(const BigInt& x_bn, const EC_Point& h_pt, const BigInt& y_bn) const {
         auto x = EC_Scalar::from_bigint(*this, x_bn);
         auto y = EC_Scalar::from_bigint(*this, y_bn);
         auto h = EC_AffinePoint(*this, h_pt);

         const Mul2Table gh_mul(h);

         if(auto r = gh_mul.mul2_vartime(x, y)) {
            return r->to_legacy_point();
         } else {
            return EC_AffinePoint::identity(*this).to_legacy_point();
         }
      }

      /**
      * Blinded point multiplication, attempts resistance to side channels
      * @param k_bn the scalar
      * @param rng a random number generator
      * @return base_point*k
      */
      BOTAN_DEPRECATED("Use EC_AffinePoint and EC_Scalar")
      EC_Point blinded_base_point_multiply(const BigInt& k_bn,
                                           RandomNumberGenerator& rng,
                                           std::vector<BigInt>& /*ws*/) const {
         auto k = EC_Scalar::from_bigint(*this, k_bn);
         auto pt = EC_AffinePoint::g_mul(k, rng);
         return pt.to_legacy_point();
      }

      /**
      * Blinded point multiplication, attempts resistance to side channels
      * Returns just the x coordinate of the point
      *
      * @param k_bn the scalar
      * @param rng a random number generator
      * @return x coordinate of base_point*k
      */
      BOTAN_DEPRECATED("Use EC_AffinePoint and EC_Scalar")
      BigInt blinded_base_point_multiply_x(const BigInt& k_bn,
                                           RandomNumberGenerator& rng,
                                           std::vector<BigInt>& /*ws*/) const {
         auto k = EC_Scalar::from_bigint(*this, k_bn);
         return BigInt(EC_AffinePoint::g_mul(k, rng).x_bytes());
      }

      /**
      * Blinded point multiplication, attempts resistance to side channels
      * @param point input point
      * @param k_bn the scalar
      * @param rng a random number generator
      * @return point*k
      */
      BOTAN_DEPRECATED("Use EC_AffinePoint and EC_Scalar")
      EC_Point blinded_var_point_multiply(const EC_Point& point,
                                          const BigInt& k_bn,
                                          RandomNumberGenerator& rng,
                                          std::vector<BigInt>& /*ws*/) const {
         auto k = EC_Scalar::from_bigint(*this, k_bn);
         auto pt = EC_AffinePoint(*this, point);
         return pt.mul(k, rng).to_legacy_point();
      }

      /**
      * Return a random scalar ie an integer in [1,order)
      */
      BOTAN_DEPRECATED("Use EC_Scalar::random") BigInt random_scalar(RandomNumberGenerator& rng) const {
         return EC_Scalar::random(*this, rng).to_bigint();
      }

      /**
      * Hash onto the curve.
      * For some curve types no mapping is currently available, in this
      * case this function will throw an exception.
      *
      * @param hash_fn the hash function to use (typically "SHA-256" or "SHA-512")
      * @param input the input to hash
      * @param input_len length of input in bytes
      * @param domain_sep a domain separator
      * @param domain_sep_len length of domain_sep in bytes
      * @param random_oracle if the mapped point must be uniform (use
               "true" here unless you know what you are doing)
      */
      BOTAN_DEPRECATED("Use EC_AffinePoint")
      EC_Point hash_to_curve(std::string_view hash_fn,
                             const uint8_t input[],
                             size_t input_len,
                             const uint8_t domain_sep[],
                             size_t domain_sep_len,
                             bool random_oracle = true) const {
         auto inp = std::span{input, input_len};
         auto dst = std::span{domain_sep, domain_sep_len};

         if(random_oracle) {
            return EC_AffinePoint::hash_to_curve_ro(*this, hash_fn, inp, dst).to_legacy_point();
         } else {
            return EC_AffinePoint::hash_to_curve_nu(*this, hash_fn, inp, dst).to_legacy_point();
         }
      }

      /**
      * Hash onto the curve.
      * For some curve types no mapping is currently available, in this
      * case this function will throw an exception.
      *
      * @param hash_fn the hash function to use (typically "SHA-256" or "SHA-512")
      * @param input the input to hash
      * @param input_len length of input in bytes
      * @param domain_sep a domain separator
      * @param random_oracle if the mapped point must be uniform (use
               "true" here unless you know what you are doing)
      */
      BOTAN_DEPRECATED("Use EC_AffinePoint")
      EC_Point hash_to_curve(std::string_view hash_fn,
                             const uint8_t input[],
                             size_t input_len,
                             std::string_view domain_sep,
                             bool random_oracle = true) const {
         auto inp = std::span{input, input_len};

         if(random_oracle) {
            return EC_AffinePoint::hash_to_curve_ro(*this, hash_fn, inp, domain_sep).to_legacy_point();
         } else {
            return EC_AffinePoint::hash_to_curve_nu(*this, hash_fn, inp, domain_sep).to_legacy_point();
         }
      }

      /**
      * Return a point on this curve with the affine values x, y
      */
      BOTAN_DEPRECATED("Deprecated - use EC_AffinePoint") EC_Point point(const BigInt& x, const BigInt& y) const {
         if(auto pt = EC_AffinePoint::from_bigint_xy(*this, x, y)) {
            return pt->to_legacy_point();
         } else {
            throw Decoding_Error("Invalid x/y coordinates for elliptic curve point");
         }
      }

      /**
      * Return the zero (or infinite) point on this curve
      */
      BOTAN_DEPRECATED("Deprecated no replacement") EC_Point zero_point() const {
         return EC_AffinePoint::identity(*this).to_legacy_point();
      }
#endif

      /**
      * Return if a == -3 mod p
      */
      BOTAN_DEPRECATED("Deprecated no replacement") bool a_is_minus_3() const { return get_a() + 3 == get_p(); }

      /**
      * Return if a == 0 mod p
      */
      BOTAN_DEPRECATED("Deprecated no replacement") bool a_is_zero() const { return get_a().is_zero(); }

      /*
      * Reduce x modulo the order
      */
      BOTAN_DEPRECATED("Use EC_Scalar") BigInt mod_order(const BigInt& x) const {
         return EC_Scalar::from_bytes_mod_order(*this, x.serialize()).to_bigint();
      }

      /*
      * Return inverse of x modulo the order
      */
      BOTAN_DEPRECATED("Use EC_Scalar") BigInt inverse_mod_order(const BigInt& x) const {
         return EC_Scalar::from_bigint(*this, x).invert().to_bigint();
      }

      /*
      * Reduce (x*x) modulo the order
      */
      BOTAN_DEPRECATED("Use EC_Scalar") BigInt square_mod_order(const BigInt& x) const {
         auto xs = EC_Scalar::from_bigint(*this, x);
         xs.square_self();
         return xs.to_bigint();
      }

      /*
      * Reduce (x*y) modulo the order
      */
      BOTAN_DEPRECATED("Use EC_Scalar") BigInt multiply_mod_order(const BigInt& x, const BigInt& y) const {
         auto xs = EC_Scalar::from_bigint(*this, x);
         auto ys = EC_Scalar::from_bigint(*this, y);
         return (xs * ys).to_bigint();
      }

      /*
      * Reduce (x*y*z) modulo the order
      */
      BOTAN_DEPRECATED("Use EC_Scalar")
      BigInt multiply_mod_order(const BigInt& x, const BigInt& y, const BigInt& z) const {
         auto xs = EC_Scalar::from_bigint(*this, x);
         auto ys = EC_Scalar::from_bigint(*this, y);
         auto zs = EC_Scalar::from_bigint(*this, z);
         return (xs * ys * zs).to_bigint();
      }

      /*
      * Return x^3 modulo the order
      */
      BOTAN_DEPRECATED("Deprecated no replacement") BigInt cube_mod_order(const BigInt& x) const {
         auto xs = EC_Scalar::from_bigint(*this, x);
         return (xs * xs * xs).to_bigint();
      }

      BOTAN_DEPRECATED("Just serialize the point and check") size_t point_size(EC_Point_Format format) const {
         // Hybrid and standard format are (x,y), compressed is y, +1 format byte
         if(format == EC_Point_Format::Compressed) {
            return (1 + get_p_bytes());
         } else {
            return (1 + 2 * get_p_bytes());
         }
      }

   private:
      static EC_Group_Data_Map& ec_group_data();

      explicit EC_Group(std::shared_ptr<EC_Group_Data>&& data);

      static std::pair<std::shared_ptr<EC_Group_Data>, bool> DER_decode_EC_group(std::span<const uint8_t> der,
                                                                                 EC_Group_Source source);

      static std::shared_ptr<EC_Group_Data> load_EC_group_info(const char* p,
                                                               const char* a,
                                                               const char* b,
                                                               const char* g_x,
                                                               const char* g_y,
                                                               const char* order,
                                                               const OID& oid);

      const EC_Group_Data& data() const;

      // Member data
      std::shared_ptr<EC_Group_Data> m_data;
      bool m_explicit_encoding = false;
};

inline bool operator!=(const EC_Group& lhs, const EC_Group& rhs) {
   return !(lhs == rhs);
}

}  // namespace Botan
namespace Botan::PK_Ops {

class Encryption;
class Decryption;
class Verification;
class Signature;
class Key_Agreement;
class KEM_Encryption;
class KEM_Decryption;

}  // namespace Botan::PK_Ops


namespace Botan {

class BigInt;
class RandomNumberGenerator;

/**
* Enumeration specifying the signature format.
*
* This is mostly used for requesting DER encoding of ECDSA signatures;
* most other algorithms only support "standard".
*/
enum class Signature_Format : uint8_t {
   Standard = 0,
   DerSequence = 1,

   IEEE_1363 BOTAN_DEPRECATED("Use Standard") = Standard,
   DER_SEQUENCE BOTAN_DEPRECATED("Use DerSequence") = DerSequence,
};

/**
* Enumeration of possible operations a public key could be used for.
*
* It is possible to query if a key supports a particular operation
* type using Asymmetric_Key::supports_operation()
*/
enum class PublicKeyOperation : uint8_t {
   Encryption,
   Signature,
   KeyEncapsulation,
   KeyAgreement,
};

class Private_Key;

/**
* An interface for objects that are keys in public key algorithms
*
* This is derived for both public and private keys
*/
class BOTAN_PUBLIC_API(3, 0) Asymmetric_Key /* NOLINT(*special-member-functions) */ {
   public:
      virtual ~Asymmetric_Key() = default;

      /**
      * Get the name of the underlying public key scheme.
      * @return name of the public key scheme
      */
      virtual std::string algo_name() const = 0;

      /**
      * Return the estimated strength of the underlying key against
      * the best currently known attack. Note that this ignores anything
      * but pure attacks against the key itself and do not take into
      * account padding schemes, usage mistakes, etc which might reduce
      * the strength. However it does suffice to provide an upper bound.
      *
      * @return estimated strength in bits
      */
      virtual size_t estimated_strength() const = 0;

      /**
      * Get the OID of the underlying public key scheme.
      * @return OID of the public key scheme
      */
      virtual OID object_identifier() const;

      /**
      * Access an algorithm specific field
      *
      * If the field is not known for this algorithm, an Invalid_Argument is
      * thrown. The interpretation of the result requires knowledge of which
      * algorithm is involved. For instance for RSA "p" represents one of the
      * secret primes, while for DSA "p" is the public prime.
      *
      * Some algorithms may not implement this method at all.
      *
      * This is primarily used to implement the FFI botan_pubkey_get_field
      * and botan_privkey_get_field functions.
      *
      * TODO(Botan4) Change this to return by value
      */
      virtual const BigInt& get_int_field(std::string_view field) const;

      /**
      * Return true if this key could be used for the specified type
      * of operation.
      */
      virtual bool supports_operation(PublicKeyOperation op) const = 0;

      /**
       * Generate another (cryptographically independent) key pair using the
       * same algorithm parameters as this key. This is most useful for algorithms
       * that support PublicKeyOperation::KeyAgreement to generate a fitting ephemeral
       * key pair. For other key types it might throw Not_Implemented.
       */
      virtual std::unique_ptr<Private_Key> generate_another(RandomNumberGenerator& rng) const = 0;

      /*
      * Test the key values for consistency.
      *
      * Note this function is always "best effort"; for many algorithms it is
      * not computationally possible to ensure the key is correctly formed in
      * all respects. There is always the possibility a malformed key will be
      * accepted; this is especially the case for public keys.
      *
      * @param rng rng to use for randomized testing (may be ignored)
      * @param strong whether to perform strong and lengthy version of the test,
      *        however for many algorithms this has no effect
      * @return true if the tests passed
      */
      virtual bool check_key(RandomNumberGenerator& rng, bool strong) const = 0;

      // Declarations for internal library functions not covered by SemVer follow

      /**
      * Certain signatures schemes such as ECDSA have more than
      * one element, and certain unfortunate protocols decided the
      * thing to do was not concatenate them as normally done, but
      * instead DER encode each of the elements as independent values.
      *
      * If this returns a value x then the signature is checked to
      * be exactly 2*x bytes and split in half for DER encoding.
      */
      virtual std::optional<size_t> _signature_element_size_for_DER_encoding() const { return {}; }

      /*
      * Return the format normally used by this algorithm for X.509 signatures
      */
      virtual Signature_Format _default_x509_signature_format() const;
};

/*
* Public Key Base Class.
*/
class BOTAN_PUBLIC_API(2, 0) Public_Key : public virtual Asymmetric_Key {
   public:
      /**
      * Return an integer value best approximating the length of the
      * primary security parameter. For example for RSA this will be
      * the size of the modulus, for ECDSA the size of the ECC group,
      * and for McEliece the size of the code will be returned.
      */
      virtual size_t key_length() const = 0;

      /**
      * Deprecated version of object_identifier
      */
      BOTAN_DEPRECATED("Use object_identifier") OID get_oid() const { return this->object_identifier(); }

      /**
      * @return X.509 AlgorithmIdentifier for this key
      */
      virtual AlgorithmIdentifier algorithm_identifier() const = 0;

      /**
      * @return binary public key bits, with no additional encoding
      *
      * For key agreements this is an alias for PK_Key_Agreement_Key::public_value.
      *
      * Note: some algorithms (for example RSA) do not have an obvious encoding
      * for this value due to having many different values, and thus throw
      * Not_Implemented when invoking this method.
      */
      virtual std::vector<uint8_t> raw_public_key_bits() const = 0;

      /**
      * @return BER encoded public key bits
      */
      virtual std::vector<uint8_t> public_key_bits() const = 0;

      /**
      * @return X.509 subject key encoding for this key object
      */
      std::vector<uint8_t> subject_public_key() const;

      /**
       * @return Hash of the subject public key
       */
      std::string fingerprint_public(std::string_view alg = "SHA-256") const;

      // Declarations for internal library functions not covered by SemVer follow

      /**
      * Returns more than 1 if the output of this algorithm
      * (ciphertext, signature) should be treated as more than one
      * value. This is used for algorithms like DSA and ECDSA, where
      * the (r,s) output pair can be encoded as either a plain binary
      * list or a TLV tagged DER encoding depending on the protocol.
      *
      * This function is public but applications should have few
      * reasons to ever call this.
      *
      * @return number of message parts
      */
      BOTAN_DEPRECATED("Deprecated no replacement") size_t message_parts() const {
         return _signature_element_size_for_DER_encoding() ? 2 : 1;
      }

      /**
      * Returns how large each of the message parts referred to
      * by message_parts() is
      *
      * This function is public but applications should have few
      * reasons to ever call this.
      *
      * @return size of the message parts in bits
      */
      BOTAN_DEPRECATED("Deprecated no replacement") size_t message_part_size() const {
         return _signature_element_size_for_DER_encoding().value_or(0);
      }

      // NOLINTBEGIN(bugprone-virtual-near-miss)

      /*
      * Return the format normally used by this algorithm for X.509 signatures
      */
      BOTAN_DEPRECATED("Deprecated no replacement") Signature_Format default_x509_signature_format() const {
         return _default_x509_signature_format();
      }

      // NOLINTEND(bugprone-virtual-near-miss)

      /**
      * This is an internal library function exposed on key types.
      * In almost all cases applications should use wrappers in pubkey.h
      *
      * Return an encryption operation for this key/params or throw
      *
      * @param rng a random number generator. The PK_Op may maintain a
      * reference to the RNG and use it many times. The rng must outlive
      * any operations which reference it.
      * @param params additional parameters
      * @param provider the provider to use
      */
      virtual std::unique_ptr<PK_Ops::Encryption> create_encryption_op(RandomNumberGenerator& rng,
                                                                       std::string_view params,
                                                                       std::string_view provider) const;

      /**
      * This is an internal library function exposed on key types.
      * In almost all cases applications should use wrappers in pubkey.h
      *
      * Return a KEM encryption operation for this key/params or throw
      *
      * @param params additional parameters
      * @param provider the provider to use
      */
      virtual std::unique_ptr<PK_Ops::KEM_Encryption> create_kem_encryption_op(std::string_view params,
                                                                               std::string_view provider) const;

      /**
      * This is an internal library function exposed on key types.
      * In all cases applications should use wrappers in pubkey.h
      *
      * Return a verification operation for this key/params or throw
      * @param params additional parameters
      * @param provider the provider to use
      */
      virtual std::unique_ptr<PK_Ops::Verification> create_verification_op(std::string_view params,
                                                                           std::string_view provider) const;

      /**
      * This is an internal library function exposed on key types.
      * In all cases applications should use wrappers in pubkey.h
      *
      * Return a verification operation for this combination of key and
      * signature algorithm or throw.
      *
      * @param signature_algorithm is the X.509 algorithm identifier encoding the padding
      * scheme and hash hash function used in the signature if applicable.
      *
      * @param provider the provider to use
      */
      virtual std::unique_ptr<PK_Ops::Verification> create_x509_verification_op(
         const AlgorithmIdentifier& signature_algorithm, std::string_view provider) const;
};

/**
* Private Key Base Class
*/
class BOTAN_PUBLIC_API(2, 0) Private_Key : public virtual Public_Key {
   public:
      /**
      * @return BER encoded private key bits
      */
      virtual secure_vector<uint8_t> private_key_bits() const = 0;

      /**
      * @return binary private key bits, with no additional encoding
      *
      * Note: some algorithms (for example RSA) do not have an obvious encoding
      * for this value due to having many different values, and thus not implement
      * this function. The default implementation throws Not_Implemented
      */
      virtual secure_vector<uint8_t> raw_private_key_bits() const;

      /**
      * Allocate a new object for the public key associated with this
      * private key.
      *
      * @return public key
      */
      virtual std::unique_ptr<Public_Key> public_key() const = 0;

      /**
      * @return PKCS #8 private key encoding for this key object
      */
      secure_vector<uint8_t> private_key_info() const;

      /**
      * @return PKCS #8 AlgorithmIdentifier for this key
      * Might be different from the X.509 identifier, but normally is not
      */
      virtual AlgorithmIdentifier pkcs8_algorithm_identifier() const { return algorithm_identifier(); }

      /**
      * Indicates if this key is stateful, ie that performing a private
      * key operation requires updating the key storage.
      */
      virtual bool stateful_operation() const { return false; }

      /**
       * @brief Retrieves the number of remaining operations if this is a stateful private key.
       *
       * @returns the number of remaining operations or std::nullopt if not applicable.
       */
      virtual std::optional<uint64_t> remaining_operations() const { return std::nullopt; }

      // Declarations for internal library functions not covered by SemVer follow

      /**
       * @return Hash of the PKCS #8 encoding for this key object
       */
      std::string fingerprint_private(std::string_view alg) const;

      /**
      * This is an internal library function exposed on key types.
      * In all cases applications should use wrappers in pubkey.h
      *
      * Return an decryption operation for this key/params or throw
      *
      * @param rng a random number generator. The PK_Op may maintain a
      * reference to the RNG and use it many times. The rng must outlive
      * any operations which reference it.
      * @param params additional parameters
      * @param provider the provider to use
      *
      */
      virtual std::unique_ptr<PK_Ops::Decryption> create_decryption_op(RandomNumberGenerator& rng,
                                                                       std::string_view params,
                                                                       std::string_view provider) const;

      /**
      * This is an internal library function exposed on key types.
      * In all cases applications should use wrappers in pubkey.h
      *
      * Return a KEM decryption operation for this key/params or throw
      *
      * @param rng a random number generator. The PK_Op may maintain a
      * reference to the RNG and use it many times. The rng must outlive
      * any operations which reference it.
      * @param params additional parameters
      * @param provider the provider to use
      */
      virtual std::unique_ptr<PK_Ops::KEM_Decryption> create_kem_decryption_op(RandomNumberGenerator& rng,
                                                                               std::string_view params,
                                                                               std::string_view provider) const;

      /**
      * This is an internal library function exposed on key types.
      * In all cases applications should use wrappers in pubkey.h
      *
      * Return a signature operation for this key/params or throw
      *
      * @param rng a random number generator. The PK_Op may maintain a
      * reference to the RNG and use it many times. The rng must outlive
      * any operations which reference it.
      * @param params additional parameters
      * @param provider the provider to use
      */
      virtual std::unique_ptr<PK_Ops::Signature> create_signature_op(RandomNumberGenerator& rng,
                                                                     std::string_view params,
                                                                     std::string_view provider) const;

      /**
      * This is an internal library function exposed on key types.
      * In all cases applications should use wrappers in pubkey.h
      *
      * Return a key agreement operation for this key/params or throw
      *
      * @param rng a random number generator. The PK_Op may maintain a
      * reference to the RNG and use it many times. The rng must outlive
      * any operations which reference it.
      * @param params additional parameters
      * @param provider the provider to use
      */
      virtual std::unique_ptr<PK_Ops::Key_Agreement> create_key_agreement_op(RandomNumberGenerator& rng,
                                                                             std::string_view params,
                                                                             std::string_view provider) const;
};

/**
* PK Secret Value Derivation Key
*/
class BOTAN_PUBLIC_API(2, 0) PK_Key_Agreement_Key : public virtual Private_Key {
   public:
      /*
      * @return public component of this key
      */
      virtual std::vector<uint8_t> public_value() const = 0;
};

/**
* Hex encode the data and separate them in blocks with `:` characters
*/
std::string BOTAN_PUBLIC_API(3, 12) format_hex_fingerprint(std::span<const uint8_t> bits);

/**
* Hash the input then format that hash using format_hex_fingerprint
*/
std::string BOTAN_PUBLIC_API(3, 0) create_hex_fingerprint(std::span<const uint8_t> bits, std::string_view hash_name);

/**
* Old interface for create_hex_fingerprint added in 2.4 pre-span
*/
inline std::string create_hex_fingerprint(const uint8_t bits[], size_t len, std::string_view hash_name) {
   return create_hex_fingerprint({bits, len}, hash_name);
}

}  // namespace Botan

namespace Botan {

class EC_AffinePoint;
class EC_Point;
class EC_Group;
class EC_Scalar;
class EC_PublicKey_Data;
class EC_PrivateKey_Data;

/**
* This class represents abstract ECC public keys. When encoding a key
* via an encoder that can be accessed via the corresponding member
* functions, the key will decide upon its internally stored encoding
* information whether to encode itself with or without domain
* parameters, or using the domain parameter oid. Furthermore, a public
* key without domain parameters can be decoded. In that case, it
* cannot be used for verification until its domain parameters are set
* by calling the corresponding member function.
*/
class BOTAN_PUBLIC_API(2, 0) EC_PublicKey : public virtual Public_Key {
   public:
      EC_PublicKey(const EC_PublicKey& other) = default;
      EC_PublicKey& operator=(const EC_PublicKey& other) = default;
      EC_PublicKey(EC_PublicKey&& other) = delete;
      EC_PublicKey& operator=(EC_PublicKey&& other) = delete;
      ~EC_PublicKey() override = default;

#if defined(BOTAN_HAS_LEGACY_EC_POINT)
      /**
      * Get the public point of this key.
      * @throw Invalid_State is thrown if the
      * domain parameters of this point are not set
      * @result the public point of this key
      */
      BOTAN_DEPRECATED("Avoid accessing the point directly") const EC_Point& public_point() const;
#endif

      AlgorithmIdentifier algorithm_identifier() const override;

      std::vector<uint8_t> raw_public_key_bits() const override;

      std::vector<uint8_t> public_key_bits() const override;

      bool check_key(RandomNumberGenerator& rng, bool strong) const override;

      /**
      * Get the domain parameters of this key.
      * @throw Invalid_State is thrown if the
      * domain parameters of this point are not set
      * @result the domain parameters of this key
      */
      const EC_Group& domain() const;

      /**
      * Set the domain parameter encoding to be used when encoding this key.
      * @param enc the encoding to use
      *
      * This function is deprecated; in a future major release only namedCurve
      * encoding of domain parameters will be allowed.
      */
      BOTAN_DEPRECATED("Support for explicit point encoding is deprecated")
      void set_parameter_encoding(EC_Group_Encoding enc);

      /**
      * Set the point encoding method to be used when encoding this key.
      * @param enc the encoding to use
      */
      void set_point_encoding(EC_Point_Format enc);

      /**
      * Return the DER encoding of this keys domain in whatever format
      * is preset for this particular key
      */
      std::vector<uint8_t> DER_domain() const;

      /**
      * Get the domain parameter encoding to be used when encoding this key.
      * @result the encoding to use
      */
      EC_Group_Encoding domain_format() const { return m_domain_encoding; }

      /**
      * Get the point encoding method to be used when encoding this key.
      * @result the encoding to use
      */
      EC_Point_Format point_encoding() const { return m_point_encoding; }

      size_t key_length() const override;
      size_t estimated_strength() const override;

      const BigInt& get_int_field(std::string_view field) const override;

      const EC_AffinePoint& _public_ec_point() const;

   protected:
#if defined(BOTAN_HAS_LEGACY_EC_POINT)
      /**
      * Load a public key from the point.
      *
      * @param group EC domain parameters
      * @param pub_point public point on the curve
      */
      EC_PublicKey(const EC_Group& group, const EC_Point& pub_point);
#endif

      /**
      * Load a public key from the point.
      *
      * @param group EC domain parameters
      * @param public_key public point on the curve
      */
      EC_PublicKey(const EC_Group& group, const EC_AffinePoint& public_key);

      /**
      * Load a public key.
      * @param alg_id the X.509 algorithm identifier
      * @param key_bits DER encoded public key bits
      */
      EC_PublicKey(const AlgorithmIdentifier& alg_id, std::span<const uint8_t> key_bits);

      EC_PublicKey() = default;

      std::shared_ptr<const EC_PublicKey_Data> m_public_key;                // NOLINT(*non-private-member-variable*)
      EC_Group_Encoding m_domain_encoding = EC_Group_Encoding::NamedCurve;  // NOLINT(*non-private-member-variable*)
      EC_Point_Format m_point_encoding = EC_Point_Format::Uncompressed;     // NOLINT(*non-private-member-variable*)
};

/**
* This abstract class represents ECC private keys
*/

BOTAN_DIAGNOSTIC_PUSH
BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE

class BOTAN_PUBLIC_API(2, 0) EC_PrivateKey : public virtual EC_PublicKey,
                                             public virtual Private_Key {
   public:
      secure_vector<uint8_t> private_key_bits() const final;

      secure_vector<uint8_t> raw_private_key_bits() const final;

      bool check_key(RandomNumberGenerator& rng, bool strong) const override;

      /**
      * Get the private key value of this key object.
      * @result the private key value of this key object
      */
      const BigInt& private_value() const;

      EC_PrivateKey(const EC_PrivateKey& other) = default;
      EC_PrivateKey& operator=(const EC_PrivateKey& other) = default;
      EC_PrivateKey(EC_PrivateKey&& other) = delete;
      EC_PrivateKey& operator=(EC_PrivateKey&& other) = delete;
      ~EC_PrivateKey() override = default;

      const BigInt& get_int_field(std::string_view field) const final;

      const EC_Scalar& _private_key() const;

   protected:
      /**
      * If x=0, creates a new private key in the domain
      * using the given rng. If with_modular_inverse is set,
      * the public key will be calculated by multiplying
      * the base point with the modular inverse of
      * x (as in ECGDSA and ECKCDSA), otherwise by
      * multiplying directly with x (as in ECDSA).
      *
      * TODO: Remove, once the respective deprecated constructors of the
      *       concrete ECC algorithms is removed.
      */
      EC_PrivateKey(RandomNumberGenerator& rng,
                    const EC_Group& group,
                    const BigInt& x,
                    bool with_modular_inverse = false);

      /**
      * Creates a new private key
      *
      * If @p with_modular_inverse is set, the public key will be calculated by
      * multiplying the base point with the modular inverse of x (as in ECGDSA
      * and ECKCDSA), otherwise by multiplying directly with x (as in ECDSA).
      */
      EC_PrivateKey(RandomNumberGenerator& rng, const EC_Group& group, bool with_modular_inverse = false);

      /**
      * Load a EC private key from the secret scalar
      *
      * If @p with_modular_inverse is set, the public key will be calculated by
      * multiplying the base point with the modular inverse of x (as in ECGDSA
      * and ECKCDSA), otherwise by multiplying directly with x (as in ECDSA).
      */
      EC_PrivateKey(const EC_Group& group, const EC_Scalar& scalar, bool with_modular_inverse = false);

      /*
      * Creates a new private key object from the
      * ECPrivateKey structure given in key_bits.
      * If with_modular_inverse is set,
      * the public key will be calculated by multiplying
      * the base point with the modular inverse of
      * x (as in ECGDSA and ECKCDSA), otherwise by
      * multiplying directly with x (as in ECDSA).
      */
      EC_PrivateKey(const AlgorithmIdentifier& alg_id,
                    std::span<const uint8_t> key_bits,
                    bool with_modular_inverse = false);

      EC_PrivateKey() : m_with_modular_inverse(false) {}

      std::shared_ptr<const EC_PrivateKey_Data> m_private_key;  // NOLINT(*non-private-member-variable*)
      bool m_with_modular_inverse;                              // NOLINT(*non-private-member-variable*)
};

BOTAN_DIAGNOSTIC_POP

}  // namespace Botan

namespace Botan {

/**
* This class represents ECDSA Public Keys.
*/
class BOTAN_PUBLIC_API(2, 0) ECDSA_PublicKey : public virtual EC_PublicKey {
   public:
      /**
      * Create a public key from a given public point.
      * @param group the domain parameters associated with this key
      * @param public_key the public point defining this key
      */
      ECDSA_PublicKey(const EC_Group& group, const EC_AffinePoint& public_key) : EC_PublicKey(group, public_key) {}

#if defined(BOTAN_HAS_LEGACY_EC_POINT)
      /**
      * Create a public key from a given public point.
      * @param group the domain parameters associated with this key
      * @param public_point the public point defining this key
      */
      ECDSA_PublicKey(const EC_Group& group, const EC_Point& public_point) : EC_PublicKey(group, public_point) {}
#endif

      /**
      * Load a public key.
      * @param alg_id the X.509 algorithm identifier
      * @param key_bits DER encoded public key bits
      */
      ECDSA_PublicKey(const AlgorithmIdentifier& alg_id, std::span<const uint8_t> key_bits) :
            EC_PublicKey(alg_id, key_bits) {}

      /**
      * Recover a public key from a signature/msg pair
      * See SEC section 4.6.1
      * @param group the elliptic curve group
      * @param msg the message
      * @param r the r parameter of the signature
      * @param s the s parameter of the signature
      * @param v the recovery ID
      */
      ECDSA_PublicKey(
         const EC_Group& group, const std::vector<uint8_t>& msg, const BigInt& r, const BigInt& s, uint8_t v);

      /**
      * Get this keys algorithm name.
      * @result this keys algorithm name ("ECDSA")
      */
      std::string algo_name() const override { return "ECDSA"; }

      std::optional<size_t> _signature_element_size_for_DER_encoding() const override;

      bool supports_operation(PublicKeyOperation op) const override { return (op == PublicKeyOperation::Signature); }

      std::unique_ptr<Private_Key> generate_another(RandomNumberGenerator& rng) const override;

      uint8_t recovery_param(const std::vector<uint8_t>& msg, const BigInt& r, const BigInt& s) const;

      std::unique_ptr<PK_Ops::Verification> create_verification_op(std::string_view params,
                                                                   std::string_view provider) const override;

      std::unique_ptr<PK_Ops::Verification> create_x509_verification_op(const AlgorithmIdentifier& signature_algorithm,
                                                                        std::string_view provider) const override;

   protected:
      ECDSA_PublicKey() = default;
};

/**
* This class represents ECDSA Private Keys
*/

BOTAN_DIAGNOSTIC_PUSH
BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE

class BOTAN_PUBLIC_API(2, 0) ECDSA_PrivateKey final : public ECDSA_PublicKey,
                                                      public EC_PrivateKey {
   public:
      /**
      * Load a private key
      * @param alg_id the X.509 algorithm identifier
      * @param key_bits ECPrivateKey bits
      */
      ECDSA_PrivateKey(const AlgorithmIdentifier& alg_id, std::span<const uint8_t> key_bits) :
            EC_PrivateKey(alg_id, key_bits) {}

      /**
      * Create a private key from a given secret @p x
      * @param group curve parameters to bu used for this key
      * @param x      the private key
      */
      ECDSA_PrivateKey(const EC_Group& group, const EC_Scalar& x) : EC_PrivateKey(group, x) {}

      /**
      * Create a new private key
      * @param rng a random number generator
      * @param group parameters to used for this key
      */
      ECDSA_PrivateKey(RandomNumberGenerator& rng, const EC_Group& group) : EC_PrivateKey(rng, group) {}

      /**
      * Create a private key.
      * @param rng a random number generator
      * @param group parameters to used for this key
      * @param x the private key (if zero, generate a new random key)
      */
      BOTAN_DEPRECATED("Use one of the other constructors")
      ECDSA_PrivateKey(RandomNumberGenerator& rng, const EC_Group& group, const BigInt& x) :
            EC_PrivateKey(rng, group, x) {}

      bool check_key(RandomNumberGenerator& rng, bool strong) const override;

      std::unique_ptr<Public_Key> public_key() const override;

      std::unique_ptr<PK_Ops::Signature> create_signature_op(RandomNumberGenerator& rng,
                                                             std::string_view params,
                                                             std::string_view provider) const override;
};

BOTAN_DIAGNOSTIC_POP

}  // namespace Botan

namespace Botan {

/**
* This class represents hash function (message digest) objects
*/
class BOTAN_PUBLIC_API(2, 0) HashFunction : public Buffered_Computation {
   public:
      /**
      * Create an instance based on a name, or return null if the
      * algo/provider combination cannot be found. If provider is
      * empty then best available is chosen.
      */
      static std::unique_ptr<HashFunction> create(std::string_view algo_spec, std::string_view provider = "");

      /**
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to use
      * Throws Lookup_Error if not found.
      */
      static std::unique_ptr<HashFunction> create_or_throw(std::string_view algo_spec, std::string_view provider = "");

      /**
      * @return list of available providers for this algorithm, empty if not available
      * @param algo_spec algorithm name
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      /**
      * @return provider information about this implementation. Default is "base",
      * might also return "sse2", "avx2", "openssl", or some other arbitrary string.
      */
      virtual std::string provider() const { return "base"; }

      /**
      * Reset the state.
      */
      virtual void clear() = 0;

      /**
      * @return the hash function name
      */
      virtual std::string name() const = 0;

      /**
      * @return hash block size as defined for this algorithm
      */
      virtual size_t hash_block_size() const { return 0; }

      /**
      * Return a new hash object with the same state as *this. This
      * allows computing the hash of several messages with a common
      * prefix more efficiently than would otherwise be possible.
      *
      * This function should be called `clone` but that was already
      * used for the case of returning an uninitialized object.
      * @return new hash object
      */
      virtual std::unique_ptr<HashFunction> copy_state() const = 0;

      /**
      * @return new object representing the same algorithm as *this
      */
      virtual std::unique_ptr<HashFunction> new_object() const = 0;

      /**
      * @return new object representing the same algorithm as *this
      */
      HashFunction* clone() const { return this->new_object().release(); }
};

}  // namespace Botan

namespace Botan {

/**
* Perform hex encoding
* @param output an array of at least input_length*2 bytes
* @param input is some binary data
* @param input_length length of input in bytes
* @param uppercase should output be upper or lower case?
*/
void BOTAN_PUBLIC_API(2, 0)
   hex_encode(char output[], const uint8_t input[], size_t input_length, bool uppercase = true);

/**
* Perform hex encoding
* @param input some input
* @param input_length length of input in bytes
* @param uppercase should output be upper or lower case?
* @return hexadecimal representation of input
*/
std::string BOTAN_PUBLIC_API(2, 0) hex_encode(const uint8_t input[], size_t input_length, bool uppercase = true);

/**
* Perform hex encoding
* @param input some input
* @param uppercase should output be upper or lower case?
* @return hexadecimal representation of input
*/
inline std::string hex_encode(std::span<const uint8_t> input, bool uppercase = true) {
   return hex_encode(input.data(), input.size(), uppercase);
}

/**
* Perform hex decoding
* @param output an array of at least input_length/2 bytes
* @param input some hex input
* @param input_length length of input in bytes
* @param input_consumed is an output parameter which says how many
*        bytes of input were actually consumed. If less than
*        input_length, then the range input[consumed:length]
*        should be passed in later along with more input.
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0)
   hex_decode(uint8_t output[], const char input[], size_t input_length, size_t& input_consumed, bool ignore_ws = true);

/**
* Perform hex decoding
* @param output an array of at least input_length/2 bytes
* @param input some hex input
* @param input_length length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0)
   hex_decode(uint8_t output[], const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform hex decoding
* @param output an array of at least input_length/2 bytes
* @param input some hex input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(3, 0) hex_decode(uint8_t output[], std::string_view input, bool ignore_ws = true);

/**
* Perform hex decoding
* @param output a contiguous byte buffer of at least input_length/2 bytes
* @param input some hex input
* @param ignore_ws ignore whitespace on input; if false, throw an
*                  exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(3, 0) hex_decode(std::span<uint8_t> output, std::string_view input, bool ignore_ws = true);

/**
* Perform hex decoding
* @param input some hex input
* @param input_length the length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded hex output
*/
std::vector<uint8_t> BOTAN_PUBLIC_API(2, 0) hex_decode(const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform hex decoding
* @param input some hex input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded hex output
*/
std::vector<uint8_t> BOTAN_PUBLIC_API(3, 0) hex_decode(std::string_view input, bool ignore_ws = true);

/**
* Perform hex decoding
* @param input some hex input
* @param input_length the length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded hex output
*/
secure_vector<uint8_t> BOTAN_PUBLIC_API(2, 0)
   hex_decode_locked(const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform hex decoding
* @param input some hex input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded hex output
*/
secure_vector<uint8_t> BOTAN_PUBLIC_API(3, 0) hex_decode_locked(std::string_view input, bool ignore_ws = true);

}  // namespace Botan

#if defined(BOTAN_TARGET_OS_HAS_THREADS)
   #include <mutex>
#endif

namespace Botan {

#if defined(BOTAN_TARGET_OS_HAS_THREADS)

using mutex_type = std::mutex;
using recursive_mutex_type = std::recursive_mutex;

template <typename T>
using lock_guard_type = std::scoped_lock<T>;

#else

// No threads

class noop_mutex final {
   public:
      void lock() {}

      void unlock() {}
};

using mutex_type = noop_mutex;
using recursive_mutex_type = noop_mutex;

template <typename Mutex>
class lock_guard final {
   public:
      explicit lock_guard(Mutex& m) : m_mutex(m) { m_mutex.lock(); }

      ~lock_guard() { m_mutex.unlock(); }

      lock_guard(const lock_guard& other) = delete;
      lock_guard& operator=(const lock_guard& other) = delete;

   private:
      Mutex& m_mutex;
};

template <typename T>
using lock_guard_type = lock_guard<T>;

#endif

}  // namespace Botan

namespace Botan {

/**
* Inherited by RNGs which maintain in-process state, like HMAC_DRBG.
* On Unix these RNGs are vulnerable to problems with fork, where the
* RNG state is duplicated, and the parent and child process RNGs will
* produce identical output until one of them reseeds. Stateful_RNG
* reseeds itself whenever a fork is detected, or after a set number of
* bytes have been output.
*
* Not implemented by RNGs which access an external RNG, such as the
* system PRNG or a hardware RNG.
*/
class BOTAN_PUBLIC_API(2, 0) Stateful_RNG : public RandomNumberGenerator {
   public:
      /**
      * @param rng is a reference to some RNG which will be used
      * to perform the periodic reseeding
      * @param entropy_sources will be polled to perform reseeding periodically
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed
      */
      Stateful_RNG(RandomNumberGenerator& rng, Entropy_Sources& entropy_sources, size_t reseed_interval) :
            m_underlying_rng(&rng), m_entropy_sources(&entropy_sources), m_reseed_interval(reseed_interval) {}

      /**
      * @param rng is a reference to some RNG which will be used
      * to perform the periodic reseeding
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed
      */
      Stateful_RNG(RandomNumberGenerator& rng, size_t reseed_interval) :
            m_underlying_rng(&rng), m_reseed_interval(reseed_interval) {}

      /**
      * @param entropy_sources will be polled to perform reseeding periodically
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed
      */
      Stateful_RNG(Entropy_Sources& entropy_sources, size_t reseed_interval) :
            m_entropy_sources(&entropy_sources), m_reseed_interval(reseed_interval) {}

      /**
      * In this case, automatic reseeding is impossible
      */
      Stateful_RNG() : m_reseed_interval(0) {}

      /**
      * Consume this input and mark the RNG as initialized regardless
      * of the length of the input or the current seeded state of
      * the RNG.
      */
      void initialize_with(std::span<const uint8_t> input);

      void initialize_with(const uint8_t input[], size_t length) { this->initialize_with(std::span(input, length)); }

      bool is_seeded() const final;

      bool accepts_input() const final { return true; }

      /**
      * Mark state as requiring a reseed on next use
      */
      void force_reseed();

      void reseed_from_rng(RandomNumberGenerator& rng, size_t poll_bits = RandomNumberGenerator::DefaultPollBits) final;

      /**
      * Poll provided sources for up to poll_bits bits of entropy.
      * Returns estimate of the number of bits collected.
      */
      size_t reseed_from_sources(Entropy_Sources& srcs,
                                 size_t poll_bits = RandomNumberGenerator::DefaultPollBits) final;

      /**
      * @return intended security level of this DRBG
      */
      virtual size_t security_level() const = 0;

      /**
      * Some DRBGs have a notion of the maximum number of bytes per
      * request.  Longer requests (to randomize) will be treated as
      * multiple requests, and may initiate reseeding multiple times,
      * depending on the values of max_number_of_bytes_per_request and
      * reseed_interval(). This function returns zero if the RNG in
      * question does not have such a notion.
      *
      * @return max number of bytes per request (or zero)
      */
      virtual size_t max_number_of_bytes_per_request() const = 0;

      size_t reseed_interval() const { return m_reseed_interval; }

      void clear() final;

   protected:
      void reseed_check();

      virtual void generate_output(std::span<uint8_t> output, std::span<const uint8_t> input) = 0;

      virtual void update(std::span<const uint8_t> input) = 0;

      virtual void clear_state() = 0;

   private:
      void generate_batched_output(std::span<uint8_t> output, std::span<const uint8_t> input);

      void fill_bytes_with_input(std::span<uint8_t> output, std::span<const uint8_t> input) final;

      void reset_reseed_counter();

      mutable recursive_mutex_type m_mutex;

      // A non-owned and possibly null pointer to shared RNG
      RandomNumberGenerator* m_underlying_rng = nullptr;

      // A non-owned and possibly null pointer to a shared Entropy_Source
      Entropy_Sources* m_entropy_sources = nullptr;

      const size_t m_reseed_interval;
      uint32_t m_last_pid = 0;

      /*
      * Set to 1 after a successful seeding, then incremented.  Reset
      * to 0 by clear() or a fork. This logic is used even if
      * automatic reseeding is disabled (via m_reseed_interval = 0)
      */
      size_t m_reseed_counter = 0;
};

}  // namespace Botan

namespace Botan {

class MessageAuthenticationCode;
class Entropy_Sources;

/**
* HMAC_DRBG from NIST SP800-90A
*/
class BOTAN_PUBLIC_API(2, 0) HMAC_DRBG final : public Stateful_RNG {
   public:
      /**
      * Initialize an HMAC_DRBG instance with the given MAC as PRF (normally HMAC)
      *
      * Automatic reseeding is disabled completely, as it has no access to
      * any source for seed material.
      *
      * If a fork is detected, the RNG will be unable to reseed itself
      * in response. In this case, an exception will be thrown rather
      * than generating duplicated output.
      */
      explicit HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf);

      /**
      * Constructor taking a string for the hash
      */
      explicit HMAC_DRBG(std::string_view hmac_hash);

      /**
      * Initialize an HMAC_DRBG instance with the given MAC as PRF (normally HMAC)
      *
      * Automatic reseeding from @p underlying_rng will take place after
      * @p reseed_interval many requests or after a fork was detected.
      *
      * @param prf MAC to use as a PRF
      * @param underlying_rng is a reference to some RNG which will be used
      * to perform the periodic reseeding
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed (max. 2^24)
      * @param max_number_of_bytes_per_request requests that are in size higher
      * than max_number_of_bytes_per_request are treated as if multiple single
      * requests of max_number_of_bytes_per_request size had been made.
      * In theory SP 800-90A requires that we reject any request for a DRBG
      * output longer than max_number_of_bytes_per_request. To avoid inconveniencing
      * the caller who wants an output larger than max_number_of_bytes_per_request,
      * instead treat these requests as if multiple requests of
      * max_number_of_bytes_per_request size had been made. NIST requires for
      * HMAC_DRBG that every implementation set a value no more than 2**19 bits
      * (or 64 KiB). Together with @p reseed_interval = 1 you can enforce that for
      * example every 512 bit automatic reseeding occurs.
      */
      HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf,
                RandomNumberGenerator& underlying_rng,
                size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval,
                size_t max_number_of_bytes_per_request = 64 * 1024);

      /**
      * Initialize an HMAC_DRBG instance with the given MAC as PRF (normally HMAC)
      *
      * Automatic reseeding from @p entropy_sources will take place after
      * @p reseed_interval many requests or after a fork was detected.
      *
      * @param prf MAC to use as a PRF
      * @param entropy_sources will be polled to perform reseeding periodically
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed (max. 2^24)
      * @param max_number_of_bytes_per_request requests that are in size higher
      * than max_number_of_bytes_per_request are treated as if multiple single
      * requests of max_number_of_bytes_per_request size had been made.
      * In theory SP 800-90A requires that we reject any request for a DRBG
      * output longer than max_number_of_bytes_per_request. To avoid inconveniencing
      * the caller who wants an output larger than max_number_of_bytes_per_request,
      * instead treat these requests as if multiple requests of
      * max_number_of_bytes_per_request size had been made. NIST requires for
      * HMAC_DRBG that every implementation set a value no more than 2**19 bits
      * (or 64 KiB). Together with @p reseed_interval = 1 you can enforce that for
      * example every 512 bit automatic reseeding occurs.
      */
      HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf,
                Entropy_Sources& entropy_sources,
                size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval,
                size_t max_number_of_bytes_per_request = 64 * 1024);

      /**
      * Initialize an HMAC_DRBG instance with the given MAC as PRF (normally HMAC)
      *
      * Automatic reseeding from @p underlying_rng and @p entropy_sources
      * will take place after @p reseed_interval many requests or after
      * a fork was detected.
      *
      * @param prf MAC to use as a PRF
      * @param underlying_rng is a reference to some RNG which will be used
      * to perform the periodic reseeding
      * @param entropy_sources will be polled to perform reseeding periodically
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed (max. 2^24)
      * @param max_number_of_bytes_per_request requests that are in size higher
      * than max_number_of_bytes_per_request are treated as if multiple single
      * requests of max_number_of_bytes_per_request size had been made.
      * In theory SP 800-90A requires that we reject any request for a DRBG
      * output longer than max_number_of_bytes_per_request. To avoid inconveniencing
      * the caller who wants an output larger than max_number_of_bytes_per_request,
      * instead treat these requests as if multiple requests of
      * max_number_of_bytes_per_request size had been made. NIST requires for
      * HMAC_DRBG that every implementation set a value no more than 2**19 bits
      * (or 64 KiB). Together with @p reseed_interval = 1 you can enforce that for
      * example every 512 bit automatic reseeding occurs.
      */
      HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf,
                RandomNumberGenerator& underlying_rng,
                Entropy_Sources& entropy_sources,
                size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval,
                size_t max_number_of_bytes_per_request = 64 * 1024);

      ~HMAC_DRBG() override;

      HMAC_DRBG(const HMAC_DRBG& rng) = delete;
      HMAC_DRBG& operator=(const HMAC_DRBG& rng) = delete;

      HMAC_DRBG(HMAC_DRBG&& rng) = delete;
      HMAC_DRBG& operator=(HMAC_DRBG&& rng) = delete;

      std::string name() const override;

      size_t security_level() const override;

      size_t max_number_of_bytes_per_request() const override { return m_max_number_of_bytes_per_request; }

   private:
      void update(std::span<const uint8_t> input) override;

      void generate_output(std::span<uint8_t> output, std::span<const uint8_t> input) override;

      void clear_state() override;

      std::unique_ptr<MessageAuthenticationCode> m_mac;
      secure_vector<uint8_t> m_V;
      secure_vector<uint8_t> m_T;
      const size_t m_max_number_of_bytes_per_request;
      const size_t m_security_level;
};

}  // namespace Botan

namespace Botan {

/**
* Key Derivation Function
*/
class BOTAN_PUBLIC_API(2, 0) KDF /* NOLINT(*-special-member-functions*) */ {
   public:
      virtual ~KDF() = default;

      /**
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to choose
      * @return a null pointer if the algo/provider combination cannot be found
      */
      static std::unique_ptr<KDF> create(std::string_view algo_spec, std::string_view provider = "");

      /**
      * Create an instance based on a name, or throw if the
      * algo/provider combination cannot be found. If provider is
      * empty then best available is chosen.
      */
      static std::unique_ptr<KDF> create_or_throw(std::string_view algo_spec, std::string_view provider = "");

      /**
      * @return list of available providers for this algorithm, empty if not available
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      /**
      * @return KDF name
      */
      virtual std::string name() const = 0;

      /**
      * Derive a key
      * @param key buffer holding the derived key, must be of length key_len
      * @param key_len the desired output length in bytes
      * @param secret the secret input
      * @param secret_len size of secret in bytes
      * @param salt a diversifier
      * @param salt_len size of salt in bytes
      * @param label purpose for the derived keying material
      * @param label_len size of label in bytes
      */
      BOTAN_DEPRECATED("Use KDF::derive_key")
      void kdf(uint8_t key[],
               size_t key_len,
               const uint8_t secret[],
               size_t secret_len,
               const uint8_t salt[],
               size_t salt_len,
               const uint8_t label[],
               size_t label_len) const {
         derive_key({key, key_len}, {secret, secret_len}, {salt, salt_len}, {label, label_len});
      }

      /**
      * Derive a key
      * @param key_len the desired output length in bytes
      * @param secret the secret input
      * @param secret_len size of secret in bytes
      * @param salt a diversifier
      * @param salt_len size of salt in bytes
      * @param label purpose for the derived keying material
      * @param label_len size of label in bytes
      * @return the derived key
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      BOTAN_DEPRECATED("Use std::span or std::string_view overloads")
      T derive_key(size_t key_len,
                   const uint8_t secret[],
                   size_t secret_len,
                   const uint8_t salt[],
                   size_t salt_len,
                   const uint8_t label[] = nullptr,
                   size_t label_len = 0) const {
         return derive_key<T>(key_len, {secret, secret_len}, {salt, salt_len}, {label, label_len});
      }

      /**
      * Derive a key
      * @param key_len the desired output length in bytes
      * @param secret the secret input
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      * @return the derived key
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T derive_key(size_t key_len,
                   std::span<const uint8_t> secret,
                   std::string_view salt = "",
                   std::string_view label = "") const {
         return derive_key<T>(key_len, secret, _as_span(salt), _as_span(label));
      }

      /**
      * Derive a key
      * @param key the output buffer for the to-be-derived key
      * @param secret the secret input
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      */
      void derive_key(std::span<uint8_t> key,
                      std::span<const uint8_t> secret,
                      std::span<const uint8_t> salt,
                      std::span<const uint8_t> label) const {
         perform_kdf(key, secret, salt, label);
      }

      /**
      * Derive a key
      * @param key_len the desired output length in bytes
      * @param secret the secret input
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      * @return the derived key
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T derive_key(size_t key_len,
                   std::span<const uint8_t> secret,
                   std::span<const uint8_t> salt,
                   std::span<const uint8_t> label) const {
         T key(key_len);
         perform_kdf(key, secret, salt, label);
         return key;
      }

      /**
      * Derive a key
      * @param key_len the desired output length in bytes
      * @param secret the secret input
      * @param salt a diversifier
      * @param salt_len size of salt in bytes
      * @param label purpose for the derived keying material
      * @return the derived key
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      BOTAN_DEPRECATED("Use std::span or std::string_view overloads")
      T derive_key(size_t key_len,
                   std::span<const uint8_t> secret,
                   const uint8_t salt[],
                   size_t salt_len,
                   std::string_view label = "") const {
         return derive_key<T>(key_len, secret, {salt, salt_len}, _as_span(label));
      }

      /**
      * Derive a key
      * @param key_len the desired output length in bytes
      * @param secret the secret input
      * @param secret_len size of secret in bytes
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      * @return the derived key
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      BOTAN_DEPRECATED("Use std::span or std::string_view overloads")
      T derive_key(size_t key_len,
                   const uint8_t secret[],
                   size_t secret_len,
                   std::string_view salt = "",
                   std::string_view label = "") const {
         return derive_key<T>(key_len, {secret, secret_len}, _as_span(salt), _as_span(label));
      }

      /**
      * Derive a key
      * @tparam key_len the desired output length in bytes
      * @param secret the secret input
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      * @return the derived key
      */
      template <size_t key_len>
      std::array<uint8_t, key_len> derive_key(std::span<const uint8_t> secret,
                                              std::span<const uint8_t> salt = {},
                                              std::span<const uint8_t> label = {}) {
         std::array<uint8_t, key_len> key{};
         perform_kdf(key, secret, salt, label);
         return key;
      }

      /**
      * Derive a key
      * @tparam key_len the desired output length in bytes
      * @param secret the secret input
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      * @return the derived key
      */
      template <size_t key_len>
      std::array<uint8_t, key_len> derive_key(std::span<const uint8_t> secret,
                                              std::span<const uint8_t> salt = {},
                                              std::string_view label = "") {
         return derive_key<key_len>(secret, salt, _as_span(label));
      }

      /**
      * Derive a key
      * @tparam key_len the desired output length in bytes
      * @param secret the secret input
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      * @return the derived key
      */
      template <size_t key_len>
      std::array<uint8_t, key_len> derive_key(std::span<const uint8_t> secret,
                                              std::string_view salt = "",
                                              std::string_view label = "") {
         return derive_key<key_len>(secret, _as_span(salt), _as_span(label));
      }

      /**
      * @return new object representing the same algorithm as *this
      */
      virtual std::unique_ptr<KDF> new_object() const = 0;

      /**
      * @return new object representing the same algorithm as *this
      */
      KDF* clone() const { return this->new_object().release(); }

   protected:
      /**
      * Internal customization point for subclasses
      *
      * The byte size of the @p key span is the number of bytes to be produced
      * by the concrete key derivation function.
      *
      * @param key the output buffer for the to-be-derived key
      * @param secret the secret input
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      */
      virtual void perform_kdf(std::span<uint8_t> key,
                               std::span<const uint8_t> secret,
                               std::span<const uint8_t> salt,
                               std::span<const uint8_t> label) const = 0;

   private:
      static std::span<const uint8_t> _as_span(std::string_view s);
};

/**
* Factory method for KDF (key derivation function)
* @param algo_spec the name of the KDF to create
* @return pointer to newly allocated object of that type
*
* Prefer KDF::create
*/
BOTAN_DEPRECATED("Use KDF::create")

inline KDF* get_kdf(std::string_view algo_spec) {
   if(algo_spec == "Raw") {
      return nullptr;
   }

   return KDF::create_or_throw(algo_spec).release();
}

}  // namespace Botan

namespace Botan {

/**
* This class represents Message Authentication Code (MAC) objects.
*/
class BOTAN_PUBLIC_API(2, 0) MessageAuthenticationCode : public Buffered_Computation,
                                                         public SymmetricAlgorithm {
   public:
      /**
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to use
      * @return a null pointer if the algo/provider combination cannot be found
      */
      static std::unique_ptr<MessageAuthenticationCode> create(std::string_view algo_spec,
                                                               std::string_view provider = "");

      /*
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to use
      * Throws a Lookup_Error if algo/provider combination cannot be found
      */
      static std::unique_ptr<MessageAuthenticationCode> create_or_throw(std::string_view algo_spec,
                                                                        std::string_view provider = "");

      /**
      * @return list of available providers for this algorithm, empty if not available
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      /**
      * Prepare for processing a message under the specified nonce
      *
      * Most MACs neither require nor support a nonce; for these algorithms
      * calling start() is optional and calling it with anything other than
      * an empty string is an error. One MAC which *requires* a per-message
      * nonce be specified is GMAC.
      *
      * Default implementation simply rejects all non-empty nonces
      * since most hash/MAC algorithms do not support randomization
      *
      * @param nonce the message nonce bytes
      */
      void start(std::span<const uint8_t> nonce) { start_msg(nonce); }

      /**
      * Begin processing a message.
      * @param nonce the per message nonce
      * @param nonce_len length of nonce
      */
      void start(const uint8_t nonce[], size_t nonce_len) { start_msg({nonce, nonce_len}); }

      /**
      * Begin processing a message.
      */
      void start() { return start_msg({}); }

      /**
      * Verify a MAC.
      * @param in the MAC to verify as a byte array
      * @param length the length of param in
      * @return true if the MAC is valid, false otherwise
      */
      bool verify_mac(const uint8_t in[], size_t length) { return verify_mac_result({in, length}); }

      /**
      * Verify a MAC.
      * @param in the MAC to verify as a byte array
      * @return true if the MAC is valid, false otherwise
      */
      bool verify_mac(std::span<const uint8_t> in) { return verify_mac_result(in); }

      /**
      * @return new object representing the same algorithm as *this
      */
      virtual std::unique_ptr<MessageAuthenticationCode> new_object() const = 0;

      /**
      * Get a new object representing the same algorithm as *this
      */
      MessageAuthenticationCode* clone() const { return this->new_object().release(); }

      /**
      * @return provider information about this implementation. Default is "base",
      * might also return "sse2", "avx2", "openssl", or some other arbitrary string.
      */
      virtual std::string provider() const { return "base"; }

      /**
      * @return if a fresh key must be set for each message that is processed.
      *
      * This is required for certain polynomial-based MACs which are insecure
      * if a key is ever reused for two different messages.
      */
      virtual bool fresh_key_required_per_message() const { return false; }

   protected:
      /**
      * Prepare for processing a message under the specified nonce
      *
      * If the MAC does not support nonces, it should not override the default
      * implementation.
      */
      virtual void start_msg(std::span<const uint8_t> nonce);

      /**
      * Verify the MACs final result
      */
      virtual bool verify_mac_result(std::span<const uint8_t> in);
};

typedef MessageAuthenticationCode MAC;

}  // namespace Botan

BOTAN_FUTURE_INTERNAL_HEADER(range_concepts.h)

namespace Botan::ranges {

/**
 * Models a std::ranges::contiguous_range that (optionally) restricts its
 * value_type to ValueT. In other words: a stretch of contiguous memory of
 * a certain type (optional ValueT).
 */
template <typename T, typename ValueT = std::ranges::range_value_t<T>>
concept contiguous_range = std::ranges::contiguous_range<T> && std::same_as<ValueT, std::ranges::range_value_t<T>>;

/**
 * Models a std::ranges::contiguous_range that satisfies
 * std::ranges::output_range with an arbitrary value_type. In other words: a
 * stretch of contiguous memory of a certain type (optional ValueT) that can be
 * written to.
 */
template <typename T, typename ValueT = std::ranges::range_value_t<T>>
concept contiguous_output_range = contiguous_range<T, ValueT> && std::ranges::output_range<T, ValueT>;

/**
 * Models a range that can be turned into a std::span<>. Typically, this is some
 * form of ranges::contiguous_range.
 */
template <typename T>
concept spanable_range = std::constructible_from<std::span<const std::ranges::range_value_t<T>>, T>;

/**
 * Models a range that can be turned into a std::span<> with a static extent.
 * Typically, this is a std::array or a std::span derived from an array.
 */
// clang-format off
template <typename T>
concept statically_spanable_range = spanable_range<T> &&
                                    decltype(std::span{std::declval<T&>()})::extent != std::dynamic_extent;

// clang-format on

/**
 * Find the length in bytes of a given contiguous range @p r.
 */
inline constexpr size_t size_bytes(const spanable_range auto& r) {
   return std::span{r}.size_bytes();
}

/**
* Throws an exception indicating that the attempted read or write was invalid
*/
[[noreturn]] void BOTAN_UNSTABLE_API memory_region_size_violation();

/**
 * Check that a given range @p r has a certain statically-known byte length. If
 * the range's extent is known at compile time, this is a static check,
 * otherwise a runtime argument check will be added.
 *
 * @throws Invalid_Argument  if range @p r has a dynamic extent and does not
 *                           feature the expected byte length.
 */
template <size_t expected, spanable_range R>
inline constexpr void assert_exact_byte_length(const R& r) {
   const std::span s{r};
   if constexpr(statically_spanable_range<R>) {
      static_assert(s.size_bytes() == expected, "memory region does not have expected byte lengths");
   } else {
      if(s.size_bytes() != expected) {
         memory_region_size_violation();
      }
   }
}

/**
 * Check that a list of ranges (in @p r0 and @p rs) all have the same byte
 * lengths. If the first range's extent is known at compile time, this will be a
 * static check for all other ranges whose extents are known at compile time,
 * otherwise a runtime argument check will be added.
 *
 * @throws Invalid_Argument  if any range has a dynamic extent and not all
 *                           ranges feature the same byte length.
 */
template <spanable_range R0, spanable_range... Rs>
inline constexpr void assert_equal_byte_lengths(const R0& r0, const Rs&... rs)
   requires(sizeof...(Rs) > 0)
{
   const std::span s0{r0};

   if constexpr(statically_spanable_range<R0>) {
      constexpr size_t expected_size = s0.size_bytes();
      (assert_exact_byte_length<expected_size>(rs), ...);
   } else {
      const size_t expected_size = s0.size_bytes();
      const bool correct_size =
         ((std::span<const std::ranges::range_value_t<Rs>>{rs}.size_bytes() == expected_size) && ...);

      if(!correct_size) {
         memory_region_size_violation();
      }
   }
}

}  // namespace Botan::ranges

BOTAN_FUTURE_INTERNAL_HEADER(mem_ops.h)

/*
The header mem_ops.h previously included the contents of allocator.h

Library code should always include allocator.h to see these
declarations; however when we are not building the library continue to
include the header here to avoid breaking application code.
*/
#if !defined(BOTAN_IS_BEING_BUILT)
#endif

namespace Botan {

/**
* Scrub memory contents in a way that a compiler should not elide,
* using some system specific technique. Note that this function might
* not zero the memory (for example, in some hypothetical
* implementation it might combine the memory contents with the output
* of a system PRNG), but if you can detect any difference in behavior
* at runtime then the clearing is side-effecting and you can just
* use `clear_mem`.
*
* Use this function to scrub memory just before deallocating it, or on
* a stack buffer before returning from the function.
*
* @param ptr a pointer to memory to scrub
* @param n the number of bytes pointed to by ptr
*/
BOTAN_PUBLIC_API(2, 0) void secure_scrub_memory(void* ptr, size_t n);

/**
* Zero memory contents in a way that a compiler should not elide,
* using some system specific technique.
*
* @param data  the data region to be scrubbed
*/
void secure_scrub_memory(ranges::contiguous_output_range auto&& data) {
   secure_scrub_memory(std::ranges::data(data), ranges::size_bytes(data));
}

/**
* Memory comparison, input insensitive
* @param x a pointer to an array
* @param y a pointer to another array
* @param len the number of Ts in x and y
* @return 0xFF iff x[i] == y[i] forall i in [0...n) or 0x00 otherwise
*/
BOTAN_DEPRECATED("This function is deprecated, use constant_time_compare()")
BOTAN_PUBLIC_API(2, 9) uint8_t ct_compare_u8(const uint8_t x[], const uint8_t y[], size_t len);

/**
 * Memory comparison, input insensitive
 * @param x a range of bytes
 * @param y another range of bytes
 * @return true iff x and y have equal lengths and x[i] == y[i] forall i in [0...n)
 */
BOTAN_PUBLIC_API(3, 3) bool constant_time_compare(std::span<const uint8_t> x, std::span<const uint8_t> y);

/**
* Memory comparison, input insensitive
* @param x a pointer to an array
* @param y a pointer to another array
* @param len the number of Ts in x and y
* @return true iff x[i] == y[i] forall i in [0...n)
*/
inline bool constant_time_compare(const uint8_t x[], const uint8_t y[], size_t len) {
   // simply assumes that *x and *y point to len allocated bytes at least
   return constant_time_compare({x, len}, {y, len});
}

/**
* Zero out some bytes. Warning: use secure_scrub_memory instead if the
* memory is about to be freed or otherwise the compiler thinks it can
* elide the writes.
*
* @param ptr a pointer to memory to zero
* @param bytes the number of bytes to zero in ptr
*/
inline constexpr void clear_bytes(void* ptr, size_t bytes) {
   if(bytes > 0) {
      std::memset(ptr, 0, bytes);
   }
}

/**
* Zero memory before use. This simply calls memset and should not be
* used in cases where the compiler cannot see the call as a
* side-effecting operation (for example, if calling clear_mem before
* deallocating memory, the compiler would be allowed to omit the call
* to memset entirely under the as-if rule.)
*
* @param ptr a pointer to an array of Ts to zero
* @param n the number of Ts pointed to by ptr
*/
template <typename T>
inline constexpr void clear_mem(T* ptr, size_t n) {
   clear_bytes(ptr, sizeof(T) * n);
}

/**
* Zero memory before use. This simply calls memset and should not be
* used in cases where the compiler cannot see the call as a
* side-effecting operation.
*
* @param mem a contiguous range of Ts to zero
*/
template <ranges::contiguous_output_range R>
inline constexpr void clear_mem(R&& mem)  // NOLINT(*-missing-std-forward)
   requires std::is_trivially_copyable_v<std::ranges::range_value_t<R>>
{
   clear_bytes(std::ranges::data(mem), ranges::size_bytes(mem));
}

/**
* Copy memory
* @param out the destination array
* @param in the source array
* @param n the number of elements of in/out
*/
template <typename T>
   requires std::is_trivial_v<std::decay_t<T>>
inline constexpr void copy_mem(T* out, const T* in, size_t n) {
   BOTAN_ASSERT_IMPLICATION(n > 0, in != nullptr && out != nullptr, "If n > 0 then args are not null");

   if(in != nullptr && out != nullptr && n > 0) {
      std::memmove(out, in, sizeof(T) * n);
   }
}

/**
* Copy memory
* @param out the destination array
* @param in the source array
*/
template <ranges::contiguous_output_range OutR, ranges::contiguous_range InR>
   requires std::is_same_v<std::ranges::range_value_t<OutR>, std::ranges::range_value_t<InR>> &&
            std::is_trivially_copyable_v<std::ranges::range_value_t<InR>>
inline constexpr void copy_mem(OutR&& out /* NOLINT(*-std-forward) */, const InR& in) {
   ranges::assert_equal_byte_lengths(out, in);
   if(std::is_constant_evaluated()) {
      std::copy(std::ranges::begin(in), std::ranges::end(in), std::ranges::begin(out));
   } else if(ranges::size_bytes(out) > 0) {
      std::memmove(std::ranges::data(out), std::ranges::data(in), ranges::size_bytes(out));
   }
}

/**
 * Copy a range of a trivially copyable type into another range of trivially
 * copyable type of matching byte length.
 */
template <ranges::contiguous_output_range ToR, ranges::contiguous_range FromR>
   requires std::is_trivially_copyable_v<std::ranges::range_value_t<FromR>> &&
            std::is_trivially_copyable_v<std::ranges::range_value_t<ToR>>
inline constexpr void typecast_copy(ToR&& out /* NOLINT(*-std-forward) */, const FromR& in) {
   ranges::assert_equal_byte_lengths(out, in);
   std::memcpy(std::ranges::data(out), std::ranges::data(in), ranges::size_bytes(out));
}

/**
 * Copy a range of trivially copyable type into an instance of trivially
 * copyable type with matching length.
 */
template <typename ToT, ranges::contiguous_range FromR>
   requires std::is_trivially_copyable_v<std::ranges::range_value_t<FromR>> && std::is_trivially_copyable_v<ToT> &&
            (!std::ranges::range<ToT>)
inline constexpr void typecast_copy(ToT& out, const FromR& in) {
   typecast_copy(std::span<ToT, 1>(&out, 1), in);
}

/**
 * Copy an instance of trivially copyable type into a range of trivially
 * copyable type with matching length.
 */
template <ranges::contiguous_output_range ToR, typename FromT>
   requires std::is_trivially_copyable_v<FromT> &&
            (!std::ranges::range<FromT>) && std::is_trivially_copyable_v<std::ranges::range_value_t<ToR>>
inline constexpr void typecast_copy(ToR&& out /* NOLINT(*-std-forward) */, const FromT& in) {
   typecast_copy(out, std::span<const FromT, 1>(&in, 1));
}

/**
 * Create a trivial type by bit-casting a range of trivially copyable type with
 * matching length into it.
 */
template <typename ToT, ranges::contiguous_range FromR>
   requires std::is_default_constructible_v<ToT> && std::is_trivially_copyable_v<ToT> &&
            std::is_trivially_copyable_v<std::ranges::range_value_t<FromR>>
inline constexpr ToT typecast_copy(const FromR& src) {
   ToT dst;  // NOLINT(*-member-init)
   typecast_copy(dst, src);
   return dst;
}

// TODO: deprecate and replace
template <typename T>
inline constexpr void typecast_copy(uint8_t out[], T in[], size_t N)
   requires std::is_trivially_copyable_v<T>
{
   // asserts that *in and *out point to the correct amount of memory
   typecast_copy(std::span<uint8_t>(out, sizeof(T) * N), std::span<const T>(in, N));
}

// TODO: deprecate and replace
template <typename T>
inline constexpr void typecast_copy(T out[], const uint8_t in[], size_t N)
   requires std::is_trivial_v<T>
{
   // asserts that *in and *out point to the correct amount of memory
   typecast_copy(std::span<T>(out, N), std::span<const uint8_t>(in, N * sizeof(T)));
}

// TODO: deprecate and replace
template <typename T>
inline constexpr void typecast_copy(uint8_t out[], const T& in) {
   // asserts that *out points to the correct amount of memory
   typecast_copy(std::span<uint8_t, sizeof(T)>(out, sizeof(T)), in);
}

// TODO: deprecate and replace
template <typename T>
   requires std::is_trivial_v<std::decay_t<T>>
inline constexpr void typecast_copy(T& out, const uint8_t in[]) {
   // asserts that *in points to the correct amount of memory
   typecast_copy(out, std::span<const uint8_t, sizeof(T)>(in, sizeof(T)));
}

// TODO: deprecate and replace
template <typename To>
   requires std::is_trivial_v<To>
inline constexpr To typecast_copy(const uint8_t src[]) noexcept {
   // asserts that *src points to the correct amount of memory
   return typecast_copy<To>(std::span<const uint8_t, sizeof(To)>(src, sizeof(To)));
}

#if !defined(BOTAN_IS_BEING_BUILT)
/**
* Set memory to a fixed value
* @param ptr a pointer to an array of bytes
* @param n the number of Ts pointed to by ptr
* @param val the value to set each byte to
*/
BOTAN_DEPRECATED("This function is deprecated") inline constexpr void set_mem(uint8_t* ptr, size_t n, uint8_t val) {
   if(n > 0) {
      std::memset(ptr, val, n);
   }
}
#endif

#if !defined(BOTAN_IS_BEING_BUILT)
inline const uint8_t* cast_char_ptr_to_uint8(const char* s) {
   return reinterpret_cast<const uint8_t*>(s);
}

inline uint8_t* cast_char_ptr_to_uint8(char* s) {
   return reinterpret_cast<uint8_t*>(s);
}
#endif

inline const char* cast_uint8_ptr_to_char(const uint8_t* b) {
   return reinterpret_cast<const char*>(b);
}

inline char* cast_uint8_ptr_to_char(uint8_t* b) {
   return reinterpret_cast<char*>(b);
}

#if !defined(BOTAN_IS_BEING_BUILT)
/**
* Memory comparison, input insensitive
* @param p1 a pointer to an array
* @param p2 a pointer to another array
* @param n the number of Ts in p1 and p2
* @return true iff p1[i] == p2[i] forall i in [0...n)
*/
template <typename T>
BOTAN_DEPRECATED("This function is deprecated")
inline bool same_mem(const T* p1, const T* p2, size_t n) {
   volatile T difference = 0;

   for(size_t i = 0; i != n; ++i) {
      difference = difference | (p1[i] ^ p2[i]);
   }

   return difference == 0;
}
#endif

#if !defined(BOTAN_IS_BEING_BUILT)

template <typename T, typename Alloc>
BOTAN_DEPRECATED("The buffer_insert functions are deprecated")
size_t buffer_insert(std::vector<T, Alloc>& buf, size_t buf_offset, const T input[], size_t input_length) {
   BOTAN_ASSERT_NOMSG(buf_offset <= buf.size());
   const size_t to_copy = std::min(input_length, buf.size() - buf_offset);
   if(to_copy > 0) {
      copy_mem(&buf[buf_offset], input, to_copy);
   }
   return to_copy;
}

template <typename T, typename Alloc, typename Alloc2>
BOTAN_DEPRECATED("The buffer_insert functions are deprecated")
size_t buffer_insert(std::vector<T, Alloc>& buf, size_t buf_offset, const std::vector<T, Alloc2>& input) {
   BOTAN_ASSERT_NOMSG(buf_offset <= buf.size());
   const size_t to_copy = std::min(input.size(), buf.size() - buf_offset);
   if(to_copy > 0) {
      copy_mem(&buf[buf_offset], input.data(), to_copy);
   }
   return to_copy;
}

#endif

/**
* XOR arrays. Postcondition out[i] = in[i] ^ out[i] forall i = 0...length
* @param out the input/output range
* @param in the read-only input range
*/
inline constexpr void xor_buf(ranges::contiguous_output_range<uint8_t> auto&& out,
                              ranges::contiguous_range<uint8_t> auto&& in) {
   ranges::assert_equal_byte_lengths(out, in);

   std::span<uint8_t> o(out);
   std::span<const uint8_t> i(in);

   for(; o.size_bytes() >= 32; o = o.subspan(32), i = i.subspan(32)) {
      auto x = typecast_copy<std::array<uint64_t, 4>>(o.template first<32>());
      const auto y = typecast_copy<std::array<uint64_t, 4>>(i.template first<32>());

      x[0] ^= y[0];
      x[1] ^= y[1];
      x[2] ^= y[2];
      x[3] ^= y[3];

      typecast_copy(o.template first<32>(), x);
   }

   for(size_t off = 0; off != o.size_bytes(); ++off) {
      o[off] ^= i[off];
   }
}

/**
* XOR arrays. Postcondition out[i] = in1[i] ^ in2[i] forall i = 0...length
* @param out the output range
* @param in1 the first input range
* @param in2 the second input range
*/
inline constexpr void xor_buf(ranges::contiguous_output_range<uint8_t> auto&& out,
                              ranges::contiguous_range<uint8_t> auto&& in1,
                              ranges::contiguous_range<uint8_t> auto&& in2) {
   ranges::assert_equal_byte_lengths(out, in1, in2);

   std::span o{out};
   std::span i1{in1};
   std::span i2{in2};

   for(; o.size_bytes() >= 32; o = o.subspan(32), i1 = i1.subspan(32), i2 = i2.subspan(32)) {
      auto x = typecast_copy<std::array<uint64_t, 4>>(i1.template first<32>());
      const auto y = typecast_copy<std::array<uint64_t, 4>>(i2.template first<32>());

      x[0] ^= y[0];
      x[1] ^= y[1];
      x[2] ^= y[2];
      x[3] ^= y[3];

      typecast_copy(o.template first<32>(), x);
   }

   for(size_t off = 0; off != o.size_bytes(); ++off) {
      o[off] = i1[off] ^ i2[off];
   }
}

/**
* XOR arrays. Postcondition out[i] = in[i] ^ out[i] forall i = 0...length
* @param out the input/output buffer
* @param in the read-only input buffer
* @param length the length of the buffers
*/
inline void xor_buf(uint8_t out[], const uint8_t in[], size_t length) {
   // simply assumes that *out and *in point to "length" allocated bytes at least
   xor_buf(std::span{out, length}, std::span{in, length});
}

/**
* XOR arrays. Postcondition out[i] = in[i] ^ in2[i] forall i = 0...length
* @param out the output buffer
* @param in the first input buffer
* @param in2 the second input buffer
* @param length the length of the three buffers
*/
inline void xor_buf(uint8_t out[], const uint8_t in[], const uint8_t in2[], size_t length) {
   // simply assumes that *out, *in, and *in2 point to "length" allocated bytes at least
   xor_buf(std::span{out, length}, std::span{in, length}, std::span{in2, length});
}

// TODO: deprecate and replace, use .subspan()
inline void xor_buf(std::span<uint8_t> out, std::span<const uint8_t> in, size_t n) {
   BOTAN_ARG_CHECK(out.size() >= n, "output span is too small");
   BOTAN_ARG_CHECK(in.size() >= n, "input span is too small");
   xor_buf(out.first(n), in.first(n));
}

// TODO: deprecate and replace, use .subspan()
template <typename Alloc>
void xor_buf(std::vector<uint8_t, Alloc>& out, const uint8_t* in, size_t n) {
   BOTAN_ARG_CHECK(out.size() >= n, "output vector is too small");
   // simply assumes that *in points to "n" allocated bytes at least
   xor_buf(std::span{out}.first(n), std::span{in, n});
}

// TODO: deprecate and replace
template <typename Alloc, typename Alloc2>
void xor_buf(std::vector<uint8_t, Alloc>& out, const uint8_t* in, const std::vector<uint8_t, Alloc2>& in2, size_t n) {
   BOTAN_ARG_CHECK(out.size() >= n, "output vector is too small");
   BOTAN_ARG_CHECK(in2.size() >= n, "input vector is too small");
   // simply assumes that *in points to "n" allocated bytes at least
   xor_buf(std::span{out}.first(n), std::span{in, n}, std::span{in2}.first(n));
}

template <typename Alloc, typename Alloc2>
std::vector<uint8_t, Alloc>& operator^=(std::vector<uint8_t, Alloc>& out, const std::vector<uint8_t, Alloc2>& in) {
   if(out.size() < in.size()) {
      out.resize(in.size());
   }

   xor_buf(std::span{out}.first(in.size()), in);
   return out;
}

}  // namespace Botan

namespace Botan {

class RandomNumberGenerator;

/**
* Return the absolute value
* @param n an integer
* @return absolute value of n
*/
inline BigInt abs(const BigInt& n) {
   return n.abs();
}

/**
* Compute the greatest common divisor
* @param x a positive integer
* @param y a positive integer
* @return gcd(x,y)
*/
BigInt BOTAN_PUBLIC_API(2, 0) gcd(const BigInt& x, const BigInt& y);

/**
* Least common multiple
* @param x a positive integer
* @param y a positive integer
* @return z, smallest integer such that z % x == 0 and z % y == 0
*/
BOTAN_DEPRECATED("Deprecated no replacement") BigInt BOTAN_PUBLIC_API(2, 0) lcm(const BigInt& x, const BigInt& y);

/**
* @param x an integer
* @return (x*x)
*/
BOTAN_DEPRECATED("Just use x*x") BigInt BOTAN_PUBLIC_API(2, 0) square(const BigInt& x);

/**
* Modular inversion. This algorithm is const time with respect to x,
* as long as x is less than modulus. It also avoids leaking
* information about the modulus, except that it does leak which of 3
* categories the modulus is in: an odd integer, a power of 2, or some
* other even number, and if the modulus is even, leaks the power of 2
* which divides the modulus.
*
* @param x a positive integer
* @param modulus a positive integer
* @return y st (x*y) % modulus == 1 or 0 if no such value
*/
BigInt BOTAN_PUBLIC_API(2, 0) inverse_mod(const BigInt& x, const BigInt& modulus);

/**
* Compute the Jacobi symbol. If n is prime, this is equivalent
* to the Legendre symbol.
* @see http://mathworld.wolfram.com/JacobiSymbol.html
*
* @param a is a non-negative integer
* @param n is an odd integer > 1
* @return (n / m)
*/
BOTAN_DEPRECATED("Deprecated no replacement") int32_t BOTAN_PUBLIC_API(2, 0) jacobi(BigInt a, BigInt n);

/**
* Modular exponentiation
* @param b an integer base
* @param x a positive exponent
* @param m a positive modulus
* @return (b^x) % m
*/
BigInt BOTAN_PUBLIC_API(2, 0) power_mod(const BigInt& b, const BigInt& x, const BigInt& m);

/**
* Compute the square root of x modulo a prime using the Tonelli-Shanks
* algorithm. This algorithm is primarily used for EC point
* decompression which takes only public inputs, as a consequence it is
* not written to be constant-time and may leak side-channel information
* about its arguments.
*
* @param x the input
* @param p the prime modulus
* @return y such that (y*y)%p == x, or -1 if no such integer
*/
BOTAN_DEPRECATED("Deprecated no replacement")
BigInt BOTAN_PUBLIC_API(3, 0) sqrt_modulo_prime(const BigInt& x, const BigInt& p);

/**
* @param x an integer
* @return count of the low zero bits in x, or, equivalently, the
*         largest value of n such that 2^n divides x evenly. Returns
*         zero if x is equal to zero.
*/
BOTAN_DEPRECATED("Deprecated no replacement") size_t BOTAN_PUBLIC_API(2, 0) low_zero_bits(const BigInt& x);

/**
* Check for primality
*
* This uses probabilistic algorithms - there is some non-zero (but very low)
* probability that this function will return true even if *n* is actually
* composite.
*
* @param n a positive integer to test for primality
* @param rng a random number generator
* @param prob chance of false positive is bounded by 1/2**prob
* @param is_random true if n was randomly chosen by us
* @return true if all primality tests passed, otherwise false
*/
bool BOTAN_PUBLIC_API(2, 0)
   is_prime(const BigInt& n, RandomNumberGenerator& rng, size_t prob = 64, bool is_random = false);

/**
* Test if the positive integer x is a perfect square ie if there
* exists some positive integer y st y*y == x
* See FIPS 186-4 sec C.4
* @return 0 if the integer is not a perfect square, otherwise
*         returns the positive y st y*y == x
*/
BOTAN_DEPRECATED("Deprecated no replacement") BigInt BOTAN_PUBLIC_API(2, 8) is_perfect_square(const BigInt& x);

/**
* Randomly generate a prime suitable for discrete logarithm parameters
* @param rng a random number generator
* @param bits how large the resulting prime should be in bits
* @param coprime a positive integer that (prime - 1) should be coprime to
* @param equiv a non-negative number that the result should be
               equivalent to modulo equiv_mod
* @param equiv_mod the modulus equiv should be checked against
* @param prob use test so false positive is bounded by 1/2**prob
* @return random prime with the specified criteria
*/
BigInt BOTAN_PUBLIC_API(2, 0) random_prime(RandomNumberGenerator& rng,
                                           size_t bits,
                                           const BigInt& coprime = BigInt::from_u64(0),
                                           size_t equiv = 1,
                                           size_t equiv_mod = 2,
                                           size_t prob = 128);

/**
* Generate a prime suitable for RSA p/q
* @param keygen_rng a random number generator
* @param prime_test_rng a random number generator
* @param bits how large the resulting prime should be in bits (must be >= 512)
* @param coprime a positive integer that (prime - 1) should be coprime to
* @param prob use test so false positive is bounded by 1/2**prob
* @return random prime with the specified criteria
*/
BOTAN_DEPRECATED("Deprecated no replacement")
BigInt BOTAN_PUBLIC_API(2, 7) generate_rsa_prime(RandomNumberGenerator& keygen_rng,
                                                 RandomNumberGenerator& prime_test_rng,
                                                 size_t bits,
                                                 const BigInt& coprime,
                                                 size_t prob = 128);

/**
* Return a 'safe' prime, of the form p=2*q+1 with q prime
* @param rng a random number generator
* @param bits is how long the resulting prime should be
* @return prime randomly chosen from safe primes of length bits
*/
BOTAN_DEPRECATED("Deprecated no replacement")
BigInt BOTAN_PUBLIC_API(2, 0) random_safe_prime(RandomNumberGenerator& rng, size_t bits);

/**
* The size of the PRIMES[] array
*/
BOTAN_DEPRECATED("Deprecated no replacement") const size_t PRIME_TABLE_SIZE = 6541;

/**
* A const array of all odd primes less than 65535
*/
extern const uint16_t BOTAN_PUBLIC_API(2, 0) PRIMES[];

}  // namespace Botan


namespace Botan {

class Path_Validation_Restrictions;
class Certificate_Store;

namespace OCSP {

class BOTAN_PUBLIC_API(2, 0) CertID final : public ASN1_Object {
   public:
      CertID() = default;

      CertID(const X509_Certificate& issuer, const BigInt& subject_serial);

      bool is_id_for(const X509_Certificate& issuer, const X509_Certificate& subject) const;

      void encode_into(DER_Encoder& to) const override;

      void decode_from(BER_Decoder& from) override;

      const std::vector<uint8_t>& issuer_key_hash() const { return m_issuer_key_hash; }

   private:
      AlgorithmIdentifier m_hash_id;
      std::vector<uint8_t> m_issuer_dn_hash;
      std::vector<uint8_t> m_issuer_key_hash;
      BigInt m_subject_serial;
};

class BOTAN_PUBLIC_API(2, 0) SingleResponse final : public ASN1_Object {
   public:
      const CertID& certid() const { return m_certid; }

      size_t cert_status() const { return m_cert_status; }

      const X509_Time& this_update() const { return m_thisupdate; }

      const X509_Time& next_update() const { return m_nextupdate; }

      void encode_into(DER_Encoder& to) const override;

      void decode_from(BER_Decoder& from) override;

      bool has_unknown_critical_extension() const { return m_has_unknown_critical_ext; }

   private:
      CertID m_certid;
      size_t m_cert_status = 2;  // unknown
      X509_Time m_thisupdate;
      X509_Time m_nextupdate;
      bool m_has_unknown_critical_ext = false;
};

/**
* An OCSP request.
*/
class BOTAN_PUBLIC_API(2, 0) Request final {
   public:
      /**
      * Create an OCSP request.
      * @param issuer_cert issuer certificate
      * @param subject_cert subject certificate
      */
      Request(const X509_Certificate& issuer_cert, const X509_Certificate& subject_cert);

      Request(const X509_Certificate& issuer_cert, const BigInt& subject_serial);

      /**
      * @return BER-encoded OCSP request
      */
      std::vector<uint8_t> BER_encode() const;

      /**
      * @return Base64-encoded OCSP request
      */
      std::string base64_encode() const;

      /**
      * @return issuer certificate
      */
      const X509_Certificate& issuer() const { return m_issuer; }

      /**
      * @return subject certificate
      * TODO(Botan4) remove this function
      */
      const X509_Certificate& subject() const {  // NOLINT(*-convert-member-functions-to-static)
         throw Not_Implemented("Method have been deprecated");
      }

      const std::vector<uint8_t>& issuer_key_hash() const { return m_certid.issuer_key_hash(); }

   private:
      X509_Certificate m_issuer;
      CertID m_certid;
};

/**
* OCSP response status.
*
* see https://tools.ietf.org/html/rfc6960#section-4.2.1
*/
enum class Response_Status_Code : uint8_t {
   Successful = 0,
   Malformed_Request = 1,
   Internal_Error = 2,
   Try_Later = 3,
   Sig_Required = 5,
   Unauthorized = 6
};

/**
* OCSP response.
*
* Note this class is only usable as an OCSP client
*/
class BOTAN_PUBLIC_API(2, 0) Response final {
   public:
      /**
      * Create a fake OCSP response from a given status code.
      * @param status the status code the check functions will return
      */
      BOTAN_FUTURE_EXPLICIT Response(Certificate_Status_Code status);

      /**
      * Parses an OCSP response.
      * @param response_bits response bits received
      */
      BOTAN_FUTURE_EXPLICIT Response(const std::vector<uint8_t>& response_bits) :
            Response(response_bits.data(), response_bits.size()) {}

      /**
      * Parses an OCSP response.
      * @param response_bits response bits received
      * @param response_bits_len length of response in bytes
      */
      Response(const uint8_t response_bits[], size_t response_bits_len);

      /**
      * Find the certificate that signed this OCSP response from all possible
      * candidates and taking the attached certificates into account.
      *
      * @param issuer_certificate is the issuer of the certificate in question
      * @param trusted_ocsp_responders optionally, a certificate store containing
      *        additionally trusted responder certificates
      *
      * @return the certificate that signed this response or std::nullopt if not found
      */
      std::optional<X509_Certificate> find_signing_certificate(
         const X509_Certificate& issuer_certificate, const Certificate_Store* trusted_ocsp_responders = nullptr) const;

      /**
      * Check signature of the OCSP response.
      *
      * Note: It is the responsibility of the caller to verify that signing
      *       certificate is trustworthy and authorized to do so.
      *
      * @param signing_certificate the certificate that signed this response
      *                            (@sa Response::find_signing_certificate).
      *
      * @return status code indicating the validity of the signature
      */
      Certificate_Status_Code verify_signature(const X509_Certificate& signing_certificate) const;

      /**
      * Check signature of the OCSP response.
      *
      * Note: It is the responsibility of the caller to verify that signing
      *       certificate is trustworthy and authorized to do so.
      *
      * @param signing_certificate the certificate that signed this response
      *                            (@sa Response::find_signing_certificate)
      * @param restrictions on the signature validation
      *
      * @return status code indicating the validity of the signature
      */
      Certificate_Status_Code verify_signature(const X509_Certificate& signing_certificate,
                                               const Path_Validation_Restrictions& restrictions) const;

      /**
      * @return the status of the response
      */
      Response_Status_Code status() const { return m_status; }

      /**
      * @return the time this OCSP response was supposedly produced at
      */
      const X509_Time& produced_at() const { return m_produced_at; }

      /**
      * @return DN of signer, if provided in response (may be empty)
      */
      const X509_DN& signer_name() const { return m_signer_name; }

      /**
      * @return key hash, if provided in response (may be empty)
      */
      const std::vector<uint8_t>& signer_key_hash() const { return m_key_hash; }

      const std::vector<uint8_t>& raw_bits() const { return m_response_bits; }

      /**
       * Searches the OCSP response for issuer and subject certificate.
       * @param issuer issuer certificate
       * @param subject subject certificate
       * @param ref_time the reference time
       * @param max_age the maximum age the response should be considered valid
       *                if next_update is not set
       * @return OCSP status code, possible values:
       *         CERT_IS_REVOKED,
       *         OCSP_NOT_YET_VALID,
       *         OCSP_HAS_EXPIRED,
       *         OCSP_IS_TOO_OLD,
       *         OCSP_RESPONSE_GOOD,
       *         OCSP_BAD_STATUS,
       *         OCSP_CERT_NOT_LISTED
       */
      Certificate_Status_Code status_for(
         const X509_Certificate& issuer,
         const X509_Certificate& subject,
         std::chrono::system_clock::time_point ref_time = std::chrono::system_clock::now(),
         std::chrono::seconds max_age = std::chrono::seconds::zero()) const;

      /**
       * @return the certificate chain, if provided in response
       */
      const std::vector<X509_Certificate>& certificates() const { return m_certs; }

      /**
      * @return the dummy response if this is a 'fake' OCSP response otherwise std::nullopt
      */
      std::optional<Certificate_Status_Code> dummy_status() const { return m_dummy_response_status; }

   private:
      bool is_issued_by(const X509_Certificate& candidate) const;

   private:
      Response_Status_Code m_status = Response_Status_Code::Internal_Error;
      std::vector<uint8_t> m_response_bits;
      X509_Time m_produced_at;
      X509_DN m_signer_name;
      std::vector<uint8_t> m_key_hash;
      std::vector<uint8_t> m_tbs_bits;
      AlgorithmIdentifier m_sig_algo;
      std::vector<uint8_t> m_signature;
      std::vector<X509_Certificate> m_certs;

      std::vector<SingleResponse> m_responses;

      std::optional<Certificate_Status_Code> m_dummy_response_status;

      bool m_has_unknown_critical_ext = false;
};

#if defined(BOTAN_HAS_HTTP_UTIL)

/**
* Makes an online OCSP request via HTTP and returns the (unverified) OCSP response.
* @param issuer issuer certificate
* @param subject_serial the subject's serial number
* @param ocsp_responder the OCSP responder to query
* @param timeout a timeout on the HTTP request
* @return OCSP response
*/
BOTAN_PUBLIC_API(3, 0)
Response online_check(const X509_Certificate& issuer,
                      const BigInt& subject_serial,
                      std::string_view ocsp_responder,
                      std::chrono::milliseconds timeout = std::chrono::milliseconds(3000));

/**
* Makes an online OCSP request via HTTP and returns the (unverified) OCSP response.
* @param issuer issuer certificate
* @param subject subject certificate
* @param timeout a timeout on the HTTP request
* @return OCSP response
*/
BOTAN_PUBLIC_API(3, 0)
Response online_check(const X509_Certificate& issuer,
                      const X509_Certificate& subject,
                      std::chrono::milliseconds timeout = std::chrono::milliseconds(3000));

#endif

}  // namespace OCSP

}  // namespace Botan

BOTAN_DEPRECATED_HEADER("oids.h")

namespace Botan::OIDS {

/**
* Register an OID to string mapping.
* @param oid the oid to register
* @param name the name to be associated with the oid
*/
BOTAN_DEPRECATED("Use OID::register_oid") inline void add_oid(const OID& oid, std::string_view name) {
   OID::register_oid(oid, name);
}

BOTAN_DEPRECATED("Use OID::register_oid") BOTAN_UNSTABLE_API void add_oid2str(const OID& oid, std::string_view name);

BOTAN_DEPRECATED("Use OID::register_oid") BOTAN_UNSTABLE_API void add_str2oid(const OID& oid, std::string_view name);

BOTAN_DEPRECATED("Use OID::register_oid") inline void add_oidstr(const char* oidstr, const char* name) {
   OID::register_oid(OID(oidstr), name);
}

/**
* Resolve an OID
* @param oid the OID to look up
* @return name associated with this OID, or an empty string
*/
BOTAN_DEPRECATED("Use OID::human_name_or_empty") inline std::string oid2str_or_empty(const OID& oid) {
   return oid.human_name_or_empty();
}

/**
* Find the OID to a name. The lookup will be performed in the
* general OID section of the configuration.
* @param name the name to resolve
* @return OID associated with the specified name
*/
BOTAN_DEPRECATED("Use OID::from_name") inline OID str2oid_or_empty(std::string_view name) {
   return OID::from_name(name).value_or(OID());
}

BOTAN_DEPRECATED("Use OID::human_name_or_empty") inline std::string oid2str_or_throw(const OID& oid) {
   std::string s = oid.human_name_or_empty();
   if(s.empty()) {
      throw Lookup_Error("No name associated with OID " + oid.to_string());
   }
   return s;
}

BOTAN_DEPRECATED("Use OID::human_name_or_empty") inline std::string lookup(const OID& oid) {
   return oid.human_name_or_empty();
}

BOTAN_DEPRECATED("Use OID::from_name") inline OID lookup(std::string_view name) {
   return OID::from_name(name).value_or(OID());
}

}  // namespace Botan::OIDS


#if defined(_MSC_VER)
   // PKCS #11 v3.2, section 2.1 - Structure packing
   //   Cryptoki structures are packed to occupy as little space as is possible.
   //   Cryptoki structures SHALL be packed with 1-byte alignment.
   // (Also recommended in official pkcs11 header comments)
   #pragma pack(push, cryptoki, 1)
#endif

#define PKCS11_DEPRECATED  // also use deprecated PKCS #11 symbols
#include <pkcs11.h>

#if defined(_MSC_VER)
   #pragma pack(pop, cryptoki)
#endif

static_assert(
   CRYPTOKI_VERSION_MAJOR == 3 && CRYPTOKI_VERSION_MINOR == 2,
   "The Botan PKCS #11 module was implemented against PKCS #11 v3.2. Please use the correct PKCS #11 headers.");

namespace Botan {

class Dynamically_Loaded_Library;

namespace PKCS11 {

using secure_string = secure_vector<uint8_t>;

// NOLINTBEGIN(*-enum-size)

enum class AttributeType : CK_ATTRIBUTE_TYPE {
   Class = CKA_CLASS,
   Token = CKA_TOKEN,
   Private = CKA_PRIVATE,
   Label = CKA_LABEL,
   UniqueId = CKA_UNIQUE_ID,
   Application = CKA_APPLICATION,
   Value = CKA_VALUE,
   ObjectId = CKA_OBJECT_ID,
   CertificateType = CKA_CERTIFICATE_TYPE,
   Issuer = CKA_ISSUER,
   SerialNumber = CKA_SERIAL_NUMBER,
   AcIssuer = CKA_AC_ISSUER,
   Owner = CKA_OWNER,
   AttrTypes = CKA_ATTR_TYPES,
   Trusted = CKA_TRUSTED,
   CertificateCategory = CKA_CERTIFICATE_CATEGORY,
   JavaMidpSecurityDomain = CKA_JAVA_MIDP_SECURITY_DOMAIN,
   Url = CKA_URL,
   HashOfSubjectPublicKey = CKA_HASH_OF_SUBJECT_PUBLIC_KEY,
   HashOfIssuerPublicKey = CKA_HASH_OF_ISSUER_PUBLIC_KEY,
   NameHashAlgorithm = CKA_NAME_HASH_ALGORITHM,
   CheckValue = CKA_CHECK_VALUE,
   KeyType = CKA_KEY_TYPE,
   Subject = CKA_SUBJECT,
   Id = CKA_ID,
   Sensitive = CKA_SENSITIVE,
   Encrypt = CKA_ENCRYPT,
   Decrypt = CKA_DECRYPT,
   Wrap = CKA_WRAP,
   Unwrap = CKA_UNWRAP,
   Sign = CKA_SIGN,
   SignRecover = CKA_SIGN_RECOVER,
   Verify = CKA_VERIFY,
   VerifyRecover = CKA_VERIFY_RECOVER,
   Derive = CKA_DERIVE,
   StartDate = CKA_START_DATE,
   EndDate = CKA_END_DATE,
   Modulus = CKA_MODULUS,
   ModulusBits = CKA_MODULUS_BITS,
   PublicExponent = CKA_PUBLIC_EXPONENT,
   PrivateExponent = CKA_PRIVATE_EXPONENT,
   Prime1 = CKA_PRIME_1,
   Prime2 = CKA_PRIME_2,
   Exponent1 = CKA_EXPONENT_1,
   Exponent2 = CKA_EXPONENT_2,
   Coefficient = CKA_COEFFICIENT,
   PublicKeyInfo = CKA_PUBLIC_KEY_INFO,
   Prime = CKA_PRIME,
   Subprime = CKA_SUBPRIME,
   Base = CKA_BASE,
   PrimeBits = CKA_PRIME_BITS,
   SubprimeBits = CKA_SUBPRIME_BITS,
   SubPrimeBits = CKA_SUB_PRIME_BITS,
   ValueBits = CKA_VALUE_BITS,
   ValueLen = CKA_VALUE_LEN,
   Extractable = CKA_EXTRACTABLE,
   Local = CKA_LOCAL,
   NeverExtractable = CKA_NEVER_EXTRACTABLE,
   AlwaysSensitive = CKA_ALWAYS_SENSITIVE,
   KeyGenMechanism = CKA_KEY_GEN_MECHANISM,
   Modifiable = CKA_MODIFIABLE,
   Copyable = CKA_COPYABLE,
   Destroyable = CKA_DESTROYABLE,
   EcdsaParams = CKA_ECDSA_PARAMS,
   EcParams = CKA_EC_PARAMS,
   EcPoint = CKA_EC_POINT,
   SecondaryAuth = CKA_SECONDARY_AUTH,
   AuthPinFlags = CKA_AUTH_PIN_FLAGS,
   AlwaysAuthenticate = CKA_ALWAYS_AUTHENTICATE,
   WrapWithTrusted = CKA_WRAP_WITH_TRUSTED,
   WrapTemplate = CKA_WRAP_TEMPLATE,
   UnwrapTemplate = CKA_UNWRAP_TEMPLATE,
   DeriveTemplate = CKA_DERIVE_TEMPLATE,
   OtpFormat = CKA_OTP_FORMAT,
   OtpLength = CKA_OTP_LENGTH,
   OtpTimeInterval = CKA_OTP_TIME_INTERVAL,
   OtpUserFriendlyMode = CKA_OTP_USER_FRIENDLY_MODE,
   OtpChallengeRequirement = CKA_OTP_CHALLENGE_REQUIREMENT,
   OtpTimeRequirement = CKA_OTP_TIME_REQUIREMENT,
   OtpCounterRequirement = CKA_OTP_COUNTER_REQUIREMENT,
   OtpPinRequirement = CKA_OTP_PIN_REQUIREMENT,
   OtpCounter = CKA_OTP_COUNTER,
   OtpTime = CKA_OTP_TIME,
   OtpUserIdentifier = CKA_OTP_USER_IDENTIFIER,
   OtpServiceIdentifier = CKA_OTP_SERVICE_IDENTIFIER,
   OtpServiceLogo = CKA_OTP_SERVICE_LOGO,
   OtpServiceLogoType = CKA_OTP_SERVICE_LOGO_TYPE,
   Gostr3410Params = CKA_GOSTR3410_PARAMS,
   Gostr3411Params = CKA_GOSTR3411_PARAMS,
   Gost28147Params = CKA_GOST28147_PARAMS,
   HwFeatureType = CKA_HW_FEATURE_TYPE,
   ResetOnInit = CKA_RESET_ON_INIT,
   HasReset = CKA_HAS_RESET,
   PixelX = CKA_PIXEL_X,
   PixelY = CKA_PIXEL_Y,
   Resolution = CKA_RESOLUTION,
   CharRows = CKA_CHAR_ROWS,
   CharColumns = CKA_CHAR_COLUMNS,
   Color = CKA_COLOR,
   BitsPerPixel = CKA_BITS_PER_PIXEL,
   CharSets = CKA_CHAR_SETS,
   EncodingMethods = CKA_ENCODING_METHODS,
   MimeTypes = CKA_MIME_TYPES,
   MechanismType = CKA_MECHANISM_TYPE,
   RequiredCmsAttributes = CKA_REQUIRED_CMS_ATTRIBUTES,
   DefaultCmsAttributes = CKA_DEFAULT_CMS_ATTRIBUTES,
   SupportedCmsAttributes = CKA_SUPPORTED_CMS_ATTRIBUTES,
   AllowedMechanisms = CKA_ALLOWED_MECHANISMS,
   ProfileId = CKA_PROFILE_ID,
   X2ratchetBag = CKA_X2RATCHET_BAG,
   X2ratchetBagsize = CKA_X2RATCHET_BAGSIZE,
   X2ratchetBobs1stmsg = CKA_X2RATCHET_BOBS1STMSG,
   X2ratchetCkr = CKA_X2RATCHET_CKR,
   X2ratchetCks = CKA_X2RATCHET_CKS,
   X2ratchetDhp = CKA_X2RATCHET_DHP,
   X2ratchetDhr = CKA_X2RATCHET_DHR,
   X2ratchetDhs = CKA_X2RATCHET_DHS,
   X2ratchetHkr = CKA_X2RATCHET_HKR,
   X2ratchetHks = CKA_X2RATCHET_HKS,
   X2ratchetIsalice = CKA_X2RATCHET_ISALICE,
   X2ratchetNhkr = CKA_X2RATCHET_NHKR,
   X2ratchetNhks = CKA_X2RATCHET_NHKS,
   X2ratchetNr = CKA_X2RATCHET_NR,
   X2ratchetNs = CKA_X2RATCHET_NS,
   X2ratchetPns = CKA_X2RATCHET_PNS,
   X2ratchetRk = CKA_X2RATCHET_RK,
   HssLevels = CKA_HSS_LEVELS,
   HssLmsType = CKA_HSS_LMS_TYPE,
   HssLmotsType = CKA_HSS_LMOTS_TYPE,
   HssLmsTypes = CKA_HSS_LMS_TYPES,
   HssLmotsTypes = CKA_HSS_LMOTS_TYPES,
   HssKeysRemaining = CKA_HSS_KEYS_REMAINING,
   ParameterSet = CKA_PARAMETER_SET,
   ObjectValidationFlags = CKA_OBJECT_VALIDATION_FLAGS,
   ValidationType = CKA_VALIDATION_TYPE,
   ValidationVersion = CKA_VALIDATION_VERSION,
   ValidationLevel = CKA_VALIDATION_LEVEL,
   ValidationModuleId = CKA_VALIDATION_MODULE_ID,
   ValidationFlag = CKA_VALIDATION_FLAG,
   ValidationAuthorityType = CKA_VALIDATION_AUTHORITY_TYPE,
   ValidationCountry = CKA_VALIDATION_COUNTRY,
   ValidationCertificateIdentifier = CKA_VALIDATION_CERTIFICATE_IDENTIFIER,
   ValidationCertificateUri = CKA_VALIDATION_CERTIFICATE_URI,
   ValidationVendorUri = CKA_VALIDATION_VENDOR_URI,
   ValidationProfile = CKA_VALIDATION_PROFILE,
   EncapsulateTemplate = CKA_ENCAPSULATE_TEMPLATE,
   DecapsulateTemplate = CKA_DECAPSULATE_TEMPLATE,
   TrustServerAuth = CKA_TRUST_SERVER_AUTH,
   TrustClientAuth = CKA_TRUST_CLIENT_AUTH,
   TrustCodeSigning = CKA_TRUST_CODE_SIGNING,
   TrustEmailProtection = CKA_TRUST_EMAIL_PROTECTION,
   TrustIpsecIke = CKA_TRUST_IPSEC_IKE,
   TrustTimeStamping = CKA_TRUST_TIME_STAMPING,
   TrustOcspSigning = CKA_TRUST_OCSP_SIGNING,
   Encapsulate = CKA_ENCAPSULATE,
   Decapsulate = CKA_DECAPSULATE,
   HashOfCertificate = CKA_HASH_OF_CERTIFICATE,
   PublicCrc64Value = CKA_PUBLIC_CRC64_VALUE,
   Seed = CKA_SEED,
   VendorDefined = CKA_VENDOR_DEFINED,
};

enum class CertificateType : CK_CERTIFICATE_TYPE {
   X509 = CKC_X_509,
   X509AttrCert = CKC_X_509_ATTR_CERT,
   Wtls = CKC_WTLS,
   VendorDefined = CKC_VENDOR_DEFINED,
};

/// Indicates if a stored certificate is a user certificate for which the corresponding private key is available
/// on the token ("token user"), a CA certificate ("authority"), or another end-entity certificate ("other entity").
enum class CertificateCategory : CK_CERTIFICATE_CATEGORY {
   Unspecified = CK_CERTIFICATE_CATEGORY_UNSPECIFIED,
   TokenUser = CK_CERTIFICATE_CATEGORY_TOKEN_USER,
   Authority = CK_CERTIFICATE_CATEGORY_AUTHORITY,
   OtherEntity = CK_CERTIFICATE_CATEGORY_OTHER_ENTITY
};

enum class OtpParamType : CK_OTP_PARAM_TYPE {
   OtpValue = CK_OTP_VALUE,
   OtpPin = CK_OTP_PIN,
   OtpChallenge = CK_OTP_CHALLENGE,
   OtpTime = CK_OTP_TIME,
   OtpCounter = CK_OTP_COUNTER,
   OtpFlags = CK_OTP_FLAGS,
   OtpOutputLength = CK_OTP_OUTPUT_LENGTH,
   OtpOutputFormat = CK_OTP_OUTPUT_FORMAT,
};

enum class OtpFormat : CK_ULONG {
   OtpFormatDecimal = CK_OTP_FORMAT_DECIMAL,
   OtpFormatHexadecimal = CK_OTP_FORMAT_HEXADECIMAL,
   OtpFormatAlphanumeric = CK_OTP_FORMAT_ALPHANUMERIC,
   OtpFormatBinary = CK_OTP_FORMAT_BINARY,
};

enum class OtpChallengeRequirement : CK_ULONG {
   OtpParamIgnored = CK_OTP_PARAM_IGNORED,
   OtpParamOptional = CK_OTP_PARAM_OPTIONAL,
   OtpParamMandatory = CK_OTP_PARAM_MANDATORY,
};

enum class JavaMidpSecurityDomain : CK_JAVA_MIDP_SECURITY_DOMAIN {
   SecurityDomainUnspecified = CK_SECURITY_DOMAIN_UNSPECIFIED,
   SecurityDomainManufacturer = CK_SECURITY_DOMAIN_MANUFACTURER,
   SecurityDomainOperator = CK_SECURITY_DOMAIN_OPERATOR,
   SecurityDomainThirdParty = CK_SECURITY_DOMAIN_THIRD_PARTY,
};

enum class PrfDataType : CK_PRF_DATA_TYPE {
   Sp800_108IterationVariable = CK_SP800_108_ITERATION_VARIABLE,
   Sp800_108Counter = CK_SP800_108_COUNTER,
   Sp800_108DkmLength = CK_SP800_108_DKM_LENGTH,
   Sp800_108ByteArray = CK_SP800_108_BYTE_ARRAY,
   Sp800_108KeyHandle = CK_SP800_108_KEY_HANDLE,
};

enum class Sp800_108DkmLengthMethod : CK_SP800_108_DKM_LENGTH_METHOD {
   Sp800_108DkmLengthSumOfKeys = CK_SP800_108_DKM_LENGTH_SUM_OF_KEYS,
   Sp800_108DkmLengthSumOfSegments = CK_SP800_108_DKM_LENGTH_SUM_OF_SEGMENTS,
};

enum class KeyDerivation : CK_ULONG {
   Null = CKD_NULL,
   Sha1Kdf = CKD_SHA1_KDF,
   Sha1KdfAsn1 = CKD_SHA1_KDF_ASN1,
   Sha1KdfConcatenate = CKD_SHA1_KDF_CONCATENATE,
   Sha224Kdf = CKD_SHA224_KDF,
   Sha256Kdf = CKD_SHA256_KDF,
   Sha384Kdf = CKD_SHA384_KDF,
   Sha512Kdf = CKD_SHA512_KDF,
   CpdiversifyKdf = CKD_CPDIVERSIFY_KDF,
   Sha3_224Kdf = CKD_SHA3_224_KDF,
   Sha3_256Kdf = CKD_SHA3_256_KDF,
   Sha3_384Kdf = CKD_SHA3_384_KDF,
   Sha3_512Kdf = CKD_SHA3_512_KDF,
   Sha1KdfSp800 = CKD_SHA1_KDF_SP800,
   Sha224KdfSp800 = CKD_SHA224_KDF_SP800,
   Sha256KdfSp800 = CKD_SHA256_KDF_SP800,
   Sha384KdfSp800 = CKD_SHA384_KDF_SP800,
   Sha512KdfSp800 = CKD_SHA512_KDF_SP800,
   Sha3_224KdfSp800 = CKD_SHA3_224_KDF_SP800,
   Sha3_256KdfSp800 = CKD_SHA3_256_KDF_SP800,
   Sha3_384KdfSp800 = CKD_SHA3_384_KDF_SP800,
   Sha3_512KdfSp800 = CKD_SHA3_512_KDF_SP800,
   Blake2b160Kdf = CKD_BLAKE2B_160_KDF,
   Blake2b256Kdf = CKD_BLAKE2B_256_KDF,
   Blake2b384Kdf = CKD_BLAKE2B_384_KDF,
   Blake2b512Kdf = CKD_BLAKE2B_512_KDF,
};

enum class Flag : CK_FLAGS {
   None = 0,
   TokenPresent = CKF_TOKEN_PRESENT,
   RemovableDevice = CKF_REMOVABLE_DEVICE,
   HwSlot = CKF_HW_SLOT,
   Rng = CKF_RNG,
   WriteProtected = CKF_WRITE_PROTECTED,
   LoginRequired = CKF_LOGIN_REQUIRED,
   UserPinInitialized = CKF_USER_PIN_INITIALIZED,
   RestoreKeyNotNeeded = CKF_RESTORE_KEY_NOT_NEEDED,
   ClockOnToken = CKF_CLOCK_ON_TOKEN,
   ProtectedAuthenticationPath = CKF_PROTECTED_AUTHENTICATION_PATH,
   DualCryptoOperations = CKF_DUAL_CRYPTO_OPERATIONS,
   TokenInitialized = CKF_TOKEN_INITIALIZED,
   SecondaryAuthentication = CKF_SECONDARY_AUTHENTICATION,
   UserPinCountLow = CKF_USER_PIN_COUNT_LOW,
   UserPinFinalTry = CKF_USER_PIN_FINAL_TRY,
   UserPinLocked = CKF_USER_PIN_LOCKED,
   UserPinToBeChanged = CKF_USER_PIN_TO_BE_CHANGED,
   SoPinCountLow = CKF_SO_PIN_COUNT_LOW,
   SoPinFinalTry = CKF_SO_PIN_FINAL_TRY,
   SoPinLocked = CKF_SO_PIN_LOCKED,
   SoPinToBeChanged = CKF_SO_PIN_TO_BE_CHANGED,
   ErrorState = CKF_ERROR_STATE,
   SeedRandomRequired = CKF_SEED_RANDOM_REQUIRED,
   AsyncSessionSupported = CKF_ASYNC_SESSION_SUPPORTED,
   RwSession = CKF_RW_SESSION,
   SerialSession = CKF_SERIAL_SESSION,
   AsyncSession = CKF_ASYNC_SESSION,
   ArrayAttribute = CKF_ARRAY_ATTRIBUTE,
   Hw = CKF_HW,
   MessageEncrypt = CKF_MESSAGE_ENCRYPT,
   MessageDecrypt = CKF_MESSAGE_DECRYPT,
   MessageSign = CKF_MESSAGE_SIGN,
   MessageVerify = CKF_MESSAGE_VERIFY,
   MultiMessage = CKF_MULTI_MESSAGE,
   FindObjects = CKF_FIND_OBJECTS,
   Encrypt = CKF_ENCRYPT,
   Decrypt = CKF_DECRYPT,
   Digest = CKF_DIGEST,
   Sign = CKF_SIGN,
   SignRecover = CKF_SIGN_RECOVER,
   Verify = CKF_VERIFY,
   VerifyRecover = CKF_VERIFY_RECOVER,
   Generate = CKF_GENERATE,
   GenerateKeyPair = CKF_GENERATE_KEY_PAIR,
   Wrap = CKF_WRAP,
   Unwrap = CKF_UNWRAP,
   Derive = CKF_DERIVE,
   EcFP = CKF_EC_F_P,
   EcF2m = CKF_EC_F_2M,
   EcEcparameters = CKF_EC_ECPARAMETERS,
   EcOid = CKF_EC_OID,
   EcNamedcurve = CKF_EC_NAMEDCURVE,
   EcUncompress = CKF_EC_UNCOMPRESS,
   EcCompress = CKF_EC_COMPRESS,
   EcCurvename = CKF_EC_CURVENAME,
   Encapsulate = CKF_ENCAPSULATE,
   Decapsulate = CKF_DECAPSULATE,
   Extension = CKF_EXTENSION,
   EndOfMessage = CKF_END_OF_MESSAGE,
   InterfaceForkSafe = CKF_INTERFACE_FORK_SAFE,
   LibraryCantCreateOsThreads = CKF_LIBRARY_CANT_CREATE_OS_THREADS,
   OsLockingOk = CKF_OS_LOCKING_OK,
   DontBlock = CKF_DONT_BLOCK,
   NextOtp = CKF_NEXT_OTP,
   ExcludeTime = CKF_EXCLUDE_TIME,
   ExcludeCounter = CKF_EXCLUDE_COUNTER,
   ExcludeChallenge = CKF_EXCLUDE_CHALLENGE,
   ExcludePin = CKF_EXCLUDE_PIN,
   UserFriendlyOtp = CKF_USER_FRIENDLY_OTP,
   HkdfSaltNull = CKF_HKDF_SALT_NULL,
   HkdfSaltData = CKF_HKDF_SALT_DATA,
   HkdfSaltKey = CKF_HKDF_SALT_KEY,
};

inline Flag operator|(Flag a, Flag b) {
   // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange)
   return static_cast<Flag>(static_cast<CK_FLAGS>(a) | static_cast<CK_FLAGS>(b));
}

enum class GeneratorFunction : CK_GENERATOR_FUNCTION {
   NoGenerate = CKG_NO_GENERATE,
   Generate = CKG_GENERATE,
   GenerateCounter = CKG_GENERATE_COUNTER,
   GenerateRandom = CKG_GENERATE_RANDOM,
   GenerateCounterXor = CKG_GENERATE_COUNTER_XOR,
};

enum class MGF : CK_RSA_PKCS_MGF_TYPE {
   MgfUnused = 0,
   Mgf1Sha1 = CKG_MGF1_SHA1,
   Mgf1Sha256 = CKG_MGF1_SHA256,
   Mgf1Sha384 = CKG_MGF1_SHA384,
   Mgf1Sha512 = CKG_MGF1_SHA512,
   Mgf1Sha224 = CKG_MGF1_SHA224,
   Mgf1Sha3_224 = CKG_MGF1_SHA3_224,
   Mgf1Sha3_256 = CKG_MGF1_SHA3_256,
   Mgf1Sha3_384 = CKG_MGF1_SHA3_384,
   Mgf1Sha3_512 = CKG_MGF1_SHA3_512,
};

enum class HardwareType : CK_HW_FEATURE_TYPE {
   MonotonicCounter = CKH_MONOTONIC_COUNTER,
   Clock = CKH_CLOCK,
   UserInterface = CKH_USER_INTERFACE,
   VendorDefined = CKH_VENDOR_DEFINED,
};

enum class HedgeType : CK_HEDGE_TYPE {
   HedgePreferred = CKH_HEDGE_PREFERRED,
   HedgeRequired = CKH_HEDGE_REQUIRED,
   DeterministicRequired = CKH_DETERMINISTIC_REQUIRED,
};

enum class KeyType : CK_KEY_TYPE {
   Rsa = CKK_RSA,
   Dsa = CKK_DSA,
   Dh = CKK_DH,
   Ecdsa = CKK_ECDSA,
   Ec = CKK_EC,
   X942Dh = CKK_X9_42_DH,
   Kea = CKK_KEA,
   GenericSecret = CKK_GENERIC_SECRET,
   Rc2 = CKK_RC2,
   Rc4 = CKK_RC4,
   Des = CKK_DES,
   Des2 = CKK_DES2,
   Des3 = CKK_DES3,
   Cast = CKK_CAST,
   Cast3 = CKK_CAST3,
   Cast5 = CKK_CAST5,
   Cast128 = CKK_CAST128,
   Rc5 = CKK_RC5,
   Idea = CKK_IDEA,
   Skipjack = CKK_SKIPJACK,
   Baton = CKK_BATON,
   Juniper = CKK_JUNIPER,
   Cdmf = CKK_CDMF,
   Aes = CKK_AES,
   Blowfish = CKK_BLOWFISH,
   Twofish = CKK_TWOFISH,
   Securid = CKK_SECURID,
   Hotp = CKK_HOTP,
   Acti = CKK_ACTI,
   Camellia = CKK_CAMELLIA,
   Aria = CKK_ARIA,
   Md5Hmac = CKK_MD5_HMAC,
   Sha1Hmac = CKK_SHA_1_HMAC,
   Ripemd128Hmac = CKK_RIPEMD128_HMAC,
   Ripemd160Hmac = CKK_RIPEMD160_HMAC,
   Sha256Hmac = CKK_SHA256_HMAC,
   Sha384Hmac = CKK_SHA384_HMAC,
   Sha512Hmac = CKK_SHA512_HMAC,
   Sha224Hmac = CKK_SHA224_HMAC,
   Seed = CKK_SEED,
   Gostr3410 = CKK_GOSTR3410,
   Gostr3411 = CKK_GOSTR3411,
   Gost28147 = CKK_GOST28147,
   Chacha20 = CKK_CHACHA20,
   Poly1305 = CKK_POLY1305,
   AesXts = CKK_AES_XTS,
   Sha3_224Hmac = CKK_SHA3_224_HMAC,
   Sha3_256Hmac = CKK_SHA3_256_HMAC,
   Sha3_384Hmac = CKK_SHA3_384_HMAC,
   Sha3_512Hmac = CKK_SHA3_512_HMAC,
   Blake2b160Hmac = CKK_BLAKE2B_160_HMAC,
   Blake2b256Hmac = CKK_BLAKE2B_256_HMAC,
   Blake2b384Hmac = CKK_BLAKE2B_384_HMAC,
   Blake2b512Hmac = CKK_BLAKE2B_512_HMAC,
   Salsa20 = CKK_SALSA20,
   X2ratchet = CKK_X2RATCHET,
   EcEdwards = CKK_EC_EDWARDS,
   EcMontgomery = CKK_EC_MONTGOMERY,
   Hkdf = CKK_HKDF,
   Sha512_224Hmac = CKK_SHA512_224_HMAC,
   Sha512_256Hmac = CKK_SHA512_256_HMAC,
   Sha512THmac = CKK_SHA512_T_HMAC,
   Hss = CKK_HSS,
   Xmss = CKK_XMSS,
   Xmssmt = CKK_XMSSMT,
   MlKem = CKK_ML_KEM,
   MlDsa = CKK_ML_DSA,
   SlhDsa = CKK_SLH_DSA,
   VendorDefined = CKK_VENDOR_DEFINED,
};

enum class MechanismType : CK_MECHANISM_TYPE {
   RsaPkcsKeyPairGen = CKM_RSA_PKCS_KEY_PAIR_GEN,
   RsaPkcs = CKM_RSA_PKCS,
   Rsa9796 = CKM_RSA_9796,
   RsaX509 = CKM_RSA_X_509,
   Md2RsaPkcs = CKM_MD2_RSA_PKCS,
   Md5RsaPkcs = CKM_MD5_RSA_PKCS,
   Sha1RsaPkcs = CKM_SHA1_RSA_PKCS,
   Ripemd128RsaPkcs = CKM_RIPEMD128_RSA_PKCS,
   Ripemd160RsaPkcs = CKM_RIPEMD160_RSA_PKCS,
   RsaPkcsOaep = CKM_RSA_PKCS_OAEP,
   RsaX931KeyPairGen = CKM_RSA_X9_31_KEY_PAIR_GEN,
   RsaX931 = CKM_RSA_X9_31,
   Sha1RsaX931 = CKM_SHA1_RSA_X9_31,
   RsaPkcsPss = CKM_RSA_PKCS_PSS,
   Sha1RsaPkcsPss = CKM_SHA1_RSA_PKCS_PSS,
   DsaKeyPairGen = CKM_DSA_KEY_PAIR_GEN,
   Dsa = CKM_DSA,
   DsaSha1 = CKM_DSA_SHA1,
   DsaSha224 = CKM_DSA_SHA224,
   DsaSha256 = CKM_DSA_SHA256,
   DsaSha384 = CKM_DSA_SHA384,
   DsaSha512 = CKM_DSA_SHA512,
   DsaSha3_224 = CKM_DSA_SHA3_224,
   DsaSha3_256 = CKM_DSA_SHA3_256,
   DsaSha3_384 = CKM_DSA_SHA3_384,
   DsaSha3_512 = CKM_DSA_SHA3_512,
   DhPkcsKeyPairGen = CKM_DH_PKCS_KEY_PAIR_GEN,
   DhPkcsDerive = CKM_DH_PKCS_DERIVE,
   X942DhKeyPairGen = CKM_X9_42_DH_KEY_PAIR_GEN,
   X942DhDerive = CKM_X9_42_DH_DERIVE,
   X942DhHybridDerive = CKM_X9_42_DH_HYBRID_DERIVE,
   X942MqvDerive = CKM_X9_42_MQV_DERIVE,
   Sha256RsaPkcs = CKM_SHA256_RSA_PKCS,
   Sha384RsaPkcs = CKM_SHA384_RSA_PKCS,
   Sha512RsaPkcs = CKM_SHA512_RSA_PKCS,
   Sha256RsaPkcsPss = CKM_SHA256_RSA_PKCS_PSS,
   Sha384RsaPkcsPss = CKM_SHA384_RSA_PKCS_PSS,
   Sha512RsaPkcsPss = CKM_SHA512_RSA_PKCS_PSS,
   Sha224RsaPkcs = CKM_SHA224_RSA_PKCS,
   Sha224RsaPkcsPss = CKM_SHA224_RSA_PKCS_PSS,
   Sha512224 = CKM_SHA512_224,
   Sha512224Hmac = CKM_SHA512_224_HMAC,
   Sha512224HmacGeneral = CKM_SHA512_224_HMAC_GENERAL,
   Sha512224KeyDerivation = CKM_SHA512_224_KEY_DERIVATION,
   Sha512256 = CKM_SHA512_256,
   Sha512256Hmac = CKM_SHA512_256_HMAC,
   Sha512256HmacGeneral = CKM_SHA512_256_HMAC_GENERAL,
   Sha512256KeyDerivation = CKM_SHA512_256_KEY_DERIVATION,
   Sha512T = CKM_SHA512_T,
   Sha512THmac = CKM_SHA512_T_HMAC,
   Sha512THmacGeneral = CKM_SHA512_T_HMAC_GENERAL,
   Sha512TKeyDerivation = CKM_SHA512_T_KEY_DERIVATION,
   Sha3_256RsaPkcs = CKM_SHA3_256_RSA_PKCS,
   Sha3_384RsaPkcs = CKM_SHA3_384_RSA_PKCS,
   Sha3_512RsaPkcs = CKM_SHA3_512_RSA_PKCS,
   Sha3_256RsaPkcsPss = CKM_SHA3_256_RSA_PKCS_PSS,
   Sha3_384RsaPkcsPss = CKM_SHA3_384_RSA_PKCS_PSS,
   Sha3_512RsaPkcsPss = CKM_SHA3_512_RSA_PKCS_PSS,
   Sha3_224RsaPkcs = CKM_SHA3_224_RSA_PKCS,
   Sha3_224RsaPkcsPss = CKM_SHA3_224_RSA_PKCS_PSS,
   Rc2KeyGen = CKM_RC2_KEY_GEN,
   Rc2Ecb = CKM_RC2_ECB,
   Rc2Cbc = CKM_RC2_CBC,
   Rc2Mac = CKM_RC2_MAC,
   Rc2MacGeneral = CKM_RC2_MAC_GENERAL,
   Rc2CbcPad = CKM_RC2_CBC_PAD,
   Rc4KeyGen = CKM_RC4_KEY_GEN,
   Rc4 = CKM_RC4,
   DesKeyGen = CKM_DES_KEY_GEN,
   DesEcb = CKM_DES_ECB,
   DesCbc = CKM_DES_CBC,
   DesMac = CKM_DES_MAC,
   DesMacGeneral = CKM_DES_MAC_GENERAL,
   DesCbcPad = CKM_DES_CBC_PAD,
   Des2KeyGen = CKM_DES2_KEY_GEN,
   Des3KeyGen = CKM_DES3_KEY_GEN,
   Des3Ecb = CKM_DES3_ECB,
   Des3Cbc = CKM_DES3_CBC,
   Des3Mac = CKM_DES3_MAC,
   Des3MacGeneral = CKM_DES3_MAC_GENERAL,
   Des3CbcPad = CKM_DES3_CBC_PAD,
   Des3CmacGeneral = CKM_DES3_CMAC_GENERAL,
   Des3Cmac = CKM_DES3_CMAC,
   CdmfKeyGen = CKM_CDMF_KEY_GEN,
   CdmfEcb = CKM_CDMF_ECB,
   CdmfCbc = CKM_CDMF_CBC,
   CdmfMac = CKM_CDMF_MAC,
   CdmfMacGeneral = CKM_CDMF_MAC_GENERAL,
   CdmfCbcPad = CKM_CDMF_CBC_PAD,
   DesOfb64 = CKM_DES_OFB64,
   DesOfb8 = CKM_DES_OFB8,
   DesCfb64 = CKM_DES_CFB64,
   DesCfb8 = CKM_DES_CFB8,
   Md2 = CKM_MD2,
   Md2Hmac = CKM_MD2_HMAC,
   Md2HmacGeneral = CKM_MD2_HMAC_GENERAL,
   Md5 = CKM_MD5,
   Md5Hmac = CKM_MD5_HMAC,
   Md5HmacGeneral = CKM_MD5_HMAC_GENERAL,
   Sha1 = CKM_SHA_1,
   Sha1Hmac = CKM_SHA_1_HMAC,
   Sha1HmacGeneral = CKM_SHA_1_HMAC_GENERAL,
   Ripemd128 = CKM_RIPEMD128,
   Ripemd128Hmac = CKM_RIPEMD128_HMAC,
   Ripemd128HmacGeneral = CKM_RIPEMD128_HMAC_GENERAL,
   Ripemd160 = CKM_RIPEMD160,
   Ripemd160Hmac = CKM_RIPEMD160_HMAC,
   Ripemd160HmacGeneral = CKM_RIPEMD160_HMAC_GENERAL,
   Sha256 = CKM_SHA256,
   Sha256Hmac = CKM_SHA256_HMAC,
   Sha256HmacGeneral = CKM_SHA256_HMAC_GENERAL,
   Sha224 = CKM_SHA224,
   Sha224Hmac = CKM_SHA224_HMAC,
   Sha224HmacGeneral = CKM_SHA224_HMAC_GENERAL,
   Sha384 = CKM_SHA384,
   Sha384Hmac = CKM_SHA384_HMAC,
   Sha384HmacGeneral = CKM_SHA384_HMAC_GENERAL,
   Sha512 = CKM_SHA512,
   Sha512Hmac = CKM_SHA512_HMAC,
   Sha512HmacGeneral = CKM_SHA512_HMAC_GENERAL,
   SecuridKeyGen = CKM_SECURID_KEY_GEN,
   Securid = CKM_SECURID,
   HotpKeyGen = CKM_HOTP_KEY_GEN,
   Hotp = CKM_HOTP,
   Acti = CKM_ACTI,
   ActiKeyGen = CKM_ACTI_KEY_GEN,
   Sha3_256 = CKM_SHA3_256,
   Sha3_256Hmac = CKM_SHA3_256_HMAC,
   Sha3_256HmacGeneral = CKM_SHA3_256_HMAC_GENERAL,
   Sha3_256KeyGen = CKM_SHA3_256_KEY_GEN,
   Sha3_224 = CKM_SHA3_224,
   Sha3_224Hmac = CKM_SHA3_224_HMAC,
   Sha3_224HmacGeneral = CKM_SHA3_224_HMAC_GENERAL,
   Sha3_224KeyGen = CKM_SHA3_224_KEY_GEN,
   Sha3_384 = CKM_SHA3_384,
   Sha3_384Hmac = CKM_SHA3_384_HMAC,
   Sha3_384HmacGeneral = CKM_SHA3_384_HMAC_GENERAL,
   Sha3_384KeyGen = CKM_SHA3_384_KEY_GEN,
   Sha3_512 = CKM_SHA3_512,
   Sha3_512Hmac = CKM_SHA3_512_HMAC,
   Sha3_512HmacGeneral = CKM_SHA3_512_HMAC_GENERAL,
   Sha3_512KeyGen = CKM_SHA3_512_KEY_GEN,
   CastKeyGen = CKM_CAST_KEY_GEN,
   CastEcb = CKM_CAST_ECB,
   CastCbc = CKM_CAST_CBC,
   CastMac = CKM_CAST_MAC,
   CastMacGeneral = CKM_CAST_MAC_GENERAL,
   CastCbcPad = CKM_CAST_CBC_PAD,
   Cast3KeyGen = CKM_CAST3_KEY_GEN,
   Cast3Ecb = CKM_CAST3_ECB,
   Cast3Cbc = CKM_CAST3_CBC,
   Cast3Mac = CKM_CAST3_MAC,
   Cast3MacGeneral = CKM_CAST3_MAC_GENERAL,
   Cast3CbcPad = CKM_CAST3_CBC_PAD,
   Cast5KeyGen = CKM_CAST5_KEY_GEN,
   Cast128KeyGen = CKM_CAST128_KEY_GEN,
   Cast5Ecb = CKM_CAST5_ECB,
   Cast128Ecb = CKM_CAST128_ECB,
   Cast5Cbc = CKM_CAST5_CBC,
   Cast128Cbc = CKM_CAST128_CBC,
   Cast5Mac = CKM_CAST5_MAC,
   Cast128Mac = CKM_CAST128_MAC,
   Cast5MacGeneral = CKM_CAST5_MAC_GENERAL,
   Cast128MacGeneral = CKM_CAST128_MAC_GENERAL,
   Cast5CbcPad = CKM_CAST5_CBC_PAD,
   Cast128CbcPad = CKM_CAST128_CBC_PAD,
   Rc5KeyGen = CKM_RC5_KEY_GEN,
   Rc5Ecb = CKM_RC5_ECB,
   Rc5Cbc = CKM_RC5_CBC,
   Rc5Mac = CKM_RC5_MAC,
   Rc5MacGeneral = CKM_RC5_MAC_GENERAL,
   Rc5CbcPad = CKM_RC5_CBC_PAD,
   IdeaKeyGen = CKM_IDEA_KEY_GEN,
   IdeaEcb = CKM_IDEA_ECB,
   IdeaCbc = CKM_IDEA_CBC,
   IdeaMac = CKM_IDEA_MAC,
   IdeaMacGeneral = CKM_IDEA_MAC_GENERAL,
   IdeaCbcPad = CKM_IDEA_CBC_PAD,
   GenericSecretKeyGen = CKM_GENERIC_SECRET_KEY_GEN,
   ConcatenateBaseAndKey = CKM_CONCATENATE_BASE_AND_KEY,
   ConcatenateBaseAndData = CKM_CONCATENATE_BASE_AND_DATA,
   ConcatenateDataAndBase = CKM_CONCATENATE_DATA_AND_BASE,
   XorBaseAndData = CKM_XOR_BASE_AND_DATA,
   ExtractKeyFromKey = CKM_EXTRACT_KEY_FROM_KEY,
   Ssl3PreMasterKeyGen = CKM_SSL3_PRE_MASTER_KEY_GEN,
   Ssl3MasterKeyDerive = CKM_SSL3_MASTER_KEY_DERIVE,
   Ssl3KeyAndMacDerive = CKM_SSL3_KEY_AND_MAC_DERIVE,
   Ssl3MasterKeyDeriveDh = CKM_SSL3_MASTER_KEY_DERIVE_DH,
   TlsPreMasterKeyGen = CKM_TLS_PRE_MASTER_KEY_GEN,
   TlsMasterKeyDerive = CKM_TLS_MASTER_KEY_DERIVE,
   TlsKeyAndMacDerive = CKM_TLS_KEY_AND_MAC_DERIVE,
   TlsMasterKeyDeriveDh = CKM_TLS_MASTER_KEY_DERIVE_DH,
   TlsPrf = CKM_TLS_PRF,
   Ssl3Md5Mac = CKM_SSL3_MD5_MAC,
   Ssl3Sha1Mac = CKM_SSL3_SHA1_MAC,
   Md5KeyDerivation = CKM_MD5_KEY_DERIVATION,
   Md2KeyDerivation = CKM_MD2_KEY_DERIVATION,
   Sha1KeyDerivation = CKM_SHA1_KEY_DERIVATION,
   Sha256KeyDerivation = CKM_SHA256_KEY_DERIVATION,
   Sha384KeyDerivation = CKM_SHA384_KEY_DERIVATION,
   Sha512KeyDerivation = CKM_SHA512_KEY_DERIVATION,
   Sha224KeyDerivation = CKM_SHA224_KEY_DERIVATION,
   Sha3_256KeyDerivation = CKM_SHA3_256_KEY_DERIVATION,
   Sha3_224KeyDerivation = CKM_SHA3_224_KEY_DERIVATION,
   Sha3_384KeyDerivation = CKM_SHA3_384_KEY_DERIVATION,
   Sha3_512KeyDerivation = CKM_SHA3_512_KEY_DERIVATION,
   Shake128KeyDerivation = CKM_SHAKE_128_KEY_DERIVATION,
   Shake256KeyDerivation = CKM_SHAKE_256_KEY_DERIVATION,
   PbeMd2DesCbc = CKM_PBE_MD2_DES_CBC,
   PbeMd5DesCbc = CKM_PBE_MD5_DES_CBC,
   PbeMd5CastCbc = CKM_PBE_MD5_CAST_CBC,
   PbeMd5Cast3Cbc = CKM_PBE_MD5_CAST3_CBC,
   PbeMd5Cast5Cbc = CKM_PBE_MD5_CAST5_CBC,
   PbeMd5Cast128Cbc = CKM_PBE_MD5_CAST128_CBC,
   PbeSha1Cast5Cbc = CKM_PBE_SHA1_CAST5_CBC,
   PbeSha1Cast128Cbc = CKM_PBE_SHA1_CAST128_CBC,
   PbeSha1Rc4128 = CKM_PBE_SHA1_RC4_128,
   PbeSha1Rc440 = CKM_PBE_SHA1_RC4_40,
   PbeSha1Des3EdeCbc = CKM_PBE_SHA1_DES3_EDE_CBC,
   PbeSha1Des2EdeCbc = CKM_PBE_SHA1_DES2_EDE_CBC,
   PbeSha1Rc2128Cbc = CKM_PBE_SHA1_RC2_128_CBC,
   PbeSha1Rc240Cbc = CKM_PBE_SHA1_RC2_40_CBC,
   Pkcs5Pbkd2 = CKM_PKCS5_PBKD2,
   PbaSha1WithSha1Hmac = CKM_PBA_SHA1_WITH_SHA1_HMAC,
   WtlsPreMasterKeyGen = CKM_WTLS_PRE_MASTER_KEY_GEN,
   WtlsMasterKeyDerive = CKM_WTLS_MASTER_KEY_DERIVE,
   WtlsMasterKeyDeriveDhEcc = CKM_WTLS_MASTER_KEY_DERIVE_DH_ECC,
   WtlsPrf = CKM_WTLS_PRF,
   WtlsServerKeyAndMacDerive = CKM_WTLS_SERVER_KEY_AND_MAC_DERIVE,
   WtlsClientKeyAndMacDerive = CKM_WTLS_CLIENT_KEY_AND_MAC_DERIVE,
   Tls10MacServer = CKM_TLS10_MAC_SERVER,
   Tls10MacClient = CKM_TLS10_MAC_CLIENT,
   Tls12Mac = CKM_TLS12_MAC,
   Tls12Kdf = CKM_TLS12_KDF,
   Tls12MasterKeyDerive = CKM_TLS12_MASTER_KEY_DERIVE,
   Tls12KeyAndMacDerive = CKM_TLS12_KEY_AND_MAC_DERIVE,
   Tls12MasterKeyDeriveDh = CKM_TLS12_MASTER_KEY_DERIVE_DH,
   Tls12KeySafeDerive = CKM_TLS12_KEY_SAFE_DERIVE,
   TlsMac = CKM_TLS_MAC,
   TlsKdf = CKM_TLS_KDF,
   KeyWrapLynks = CKM_KEY_WRAP_LYNKS,
   KeyWrapSetOaep = CKM_KEY_WRAP_SET_OAEP,
   CmsSig = CKM_CMS_SIG,
   KipDerive = CKM_KIP_DERIVE,
   KipWrap = CKM_KIP_WRAP,
   KipMac = CKM_KIP_MAC,
   CamelliaKeyGen = CKM_CAMELLIA_KEY_GEN,
   CamelliaEcb = CKM_CAMELLIA_ECB,
   CamelliaCbc = CKM_CAMELLIA_CBC,
   CamelliaMac = CKM_CAMELLIA_MAC,
   CamelliaMacGeneral = CKM_CAMELLIA_MAC_GENERAL,
   CamelliaCbcPad = CKM_CAMELLIA_CBC_PAD,
   CamelliaEcbEncryptData = CKM_CAMELLIA_ECB_ENCRYPT_DATA,
   CamelliaCbcEncryptData = CKM_CAMELLIA_CBC_ENCRYPT_DATA,
   CamelliaCtr = CKM_CAMELLIA_CTR,
   AriaKeyGen = CKM_ARIA_KEY_GEN,
   AriaEcb = CKM_ARIA_ECB,
   AriaCbc = CKM_ARIA_CBC,
   AriaMac = CKM_ARIA_MAC,
   AriaMacGeneral = CKM_ARIA_MAC_GENERAL,
   AriaCbcPad = CKM_ARIA_CBC_PAD,
   AriaEcbEncryptData = CKM_ARIA_ECB_ENCRYPT_DATA,
   AriaCbcEncryptData = CKM_ARIA_CBC_ENCRYPT_DATA,
   SeedKeyGen = CKM_SEED_KEY_GEN,
   SeedEcb = CKM_SEED_ECB,
   SeedCbc = CKM_SEED_CBC,
   SeedMac = CKM_SEED_MAC,
   SeedMacGeneral = CKM_SEED_MAC_GENERAL,
   SeedCbcPad = CKM_SEED_CBC_PAD,
   SeedEcbEncryptData = CKM_SEED_ECB_ENCRYPT_DATA,
   SeedCbcEncryptData = CKM_SEED_CBC_ENCRYPT_DATA,
   SkipjackKeyGen = CKM_SKIPJACK_KEY_GEN,
   SkipjackEcb64 = CKM_SKIPJACK_ECB64,
   SkipjackCbc64 = CKM_SKIPJACK_CBC64,
   SkipjackOfb64 = CKM_SKIPJACK_OFB64,
   SkipjackCfb64 = CKM_SKIPJACK_CFB64,
   SkipjackCfb32 = CKM_SKIPJACK_CFB32,
   SkipjackCfb16 = CKM_SKIPJACK_CFB16,
   SkipjackCfb8 = CKM_SKIPJACK_CFB8,
   SkipjackWrap = CKM_SKIPJACK_WRAP,
   SkipjackPrivateWrap = CKM_SKIPJACK_PRIVATE_WRAP,
   SkipjackRelayx = CKM_SKIPJACK_RELAYX,
   KeaKeyPairGen = CKM_KEA_KEY_PAIR_GEN,
   KeaKeyDerive = CKM_KEA_KEY_DERIVE,
   KeaDerive = CKM_KEA_DERIVE,
   FortezzaTimestamp = CKM_FORTEZZA_TIMESTAMP,
   BatonKeyGen = CKM_BATON_KEY_GEN,
   BatonEcb128 = CKM_BATON_ECB128,
   BatonEcb96 = CKM_BATON_ECB96,
   BatonCbc128 = CKM_BATON_CBC128,
   BatonCounter = CKM_BATON_COUNTER,
   BatonShuffle = CKM_BATON_SHUFFLE,
   BatonWrap = CKM_BATON_WRAP,
   EcdsaKeyPairGen = CKM_ECDSA_KEY_PAIR_GEN,
   EcKeyPairGen = CKM_EC_KEY_PAIR_GEN,
   Ecdsa = CKM_ECDSA,
   EcdsaSha1 = CKM_ECDSA_SHA1,
   EcdsaSha224 = CKM_ECDSA_SHA224,
   EcdsaSha256 = CKM_ECDSA_SHA256,
   EcdsaSha384 = CKM_ECDSA_SHA384,
   EcdsaSha512 = CKM_ECDSA_SHA512,
   EcKeyPairGenWExtraBits = CKM_EC_KEY_PAIR_GEN_W_EXTRA_BITS,
   Ecdh1Derive = CKM_ECDH1_DERIVE,
   Ecdh1CofactorDerive = CKM_ECDH1_COFACTOR_DERIVE,
   EcmqvDerive = CKM_ECMQV_DERIVE,
   EcdhAesKeyWrap = CKM_ECDH_AES_KEY_WRAP,
   RsaAesKeyWrap = CKM_RSA_AES_KEY_WRAP,
   JuniperKeyGen = CKM_JUNIPER_KEY_GEN,
   JuniperEcb128 = CKM_JUNIPER_ECB128,
   JuniperCbc128 = CKM_JUNIPER_CBC128,
   JuniperCounter = CKM_JUNIPER_COUNTER,
   JuniperShuffle = CKM_JUNIPER_SHUFFLE,
   JuniperWrap = CKM_JUNIPER_WRAP,
   Fasthash = CKM_FASTHASH,
   AesXts = CKM_AES_XTS,
   AesXtsKeyGen = CKM_AES_XTS_KEY_GEN,
   AesKeyGen = CKM_AES_KEY_GEN,
   AesEcb = CKM_AES_ECB,
   AesCbc = CKM_AES_CBC,
   AesMac = CKM_AES_MAC,
   AesMacGeneral = CKM_AES_MAC_GENERAL,
   AesCbcPad = CKM_AES_CBC_PAD,
   AesCtr = CKM_AES_CTR,
   AesGcm = CKM_AES_GCM,
   AesCcm = CKM_AES_CCM,
   AesCts = CKM_AES_CTS,
   AesCmac = CKM_AES_CMAC,
   AesCmacGeneral = CKM_AES_CMAC_GENERAL,
   AesXcbcMac = CKM_AES_XCBC_MAC,
   AesXcbcMac96 = CKM_AES_XCBC_MAC_96,
   AesGmac = CKM_AES_GMAC,
   BlowfishKeyGen = CKM_BLOWFISH_KEY_GEN,
   BlowfishCbc = CKM_BLOWFISH_CBC,
   TwofishKeyGen = CKM_TWOFISH_KEY_GEN,
   TwofishCbc = CKM_TWOFISH_CBC,
   BlowfishCbcPad = CKM_BLOWFISH_CBC_PAD,
   TwofishCbcPad = CKM_TWOFISH_CBC_PAD,
   DesEcbEncryptData = CKM_DES_ECB_ENCRYPT_DATA,
   DesCbcEncryptData = CKM_DES_CBC_ENCRYPT_DATA,
   Des3EcbEncryptData = CKM_DES3_ECB_ENCRYPT_DATA,
   Des3CbcEncryptData = CKM_DES3_CBC_ENCRYPT_DATA,
   AesEcbEncryptData = CKM_AES_ECB_ENCRYPT_DATA,
   AesCbcEncryptData = CKM_AES_CBC_ENCRYPT_DATA,
   Gostr3410KeyPairGen = CKM_GOSTR3410_KEY_PAIR_GEN,
   Gostr3410 = CKM_GOSTR3410,
   Gostr3410WithGostr3411 = CKM_GOSTR3410_WITH_GOSTR3411,
   Gostr3410KeyWrap = CKM_GOSTR3410_KEY_WRAP,
   Gostr3410Derive = CKM_GOSTR3410_DERIVE,
   Gostr3411 = CKM_GOSTR3411,
   Gostr3411Hmac = CKM_GOSTR3411_HMAC,
   Gost28147KeyGen = CKM_GOST28147_KEY_GEN,
   Gost28147Ecb = CKM_GOST28147_ECB,
   Gost28147 = CKM_GOST28147,
   Gost28147Mac = CKM_GOST28147_MAC,
   Gost28147KeyWrap = CKM_GOST28147_KEY_WRAP,
   Chacha20KeyGen = CKM_CHACHA20_KEY_GEN,
   Chacha20 = CKM_CHACHA20,
   Poly1305KeyGen = CKM_POLY1305_KEY_GEN,
   Poly1305 = CKM_POLY1305,
   DsaParameterGen = CKM_DSA_PARAMETER_GEN,
   DhPkcsParameterGen = CKM_DH_PKCS_PARAMETER_GEN,
   X942DhParameterGen = CKM_X9_42_DH_PARAMETER_GEN,
   DsaProbablisticParameterGen = CKM_DSA_PROBABLISTIC_PARAMETER_GEN,  // TODO(Botan4) remove this typo
   DsaProbabilisticParameterGen = CKM_DSA_PROBABILISTIC_PARAMETER_GEN,
   DsaShaweTaylorParameterGen = CKM_DSA_SHAWE_TAYLOR_PARAMETER_GEN,
   DsaFipsGGen = CKM_DSA_FIPS_G_GEN,
   AesOfb = CKM_AES_OFB,
   AesCfb64 = CKM_AES_CFB64,
   AesCfb8 = CKM_AES_CFB8,
   AesCfb128 = CKM_AES_CFB128,
   AesCfb1 = CKM_AES_CFB1,
   AesKeyWrap = CKM_AES_KEY_WRAP,
   AesKeyWrapPad = CKM_AES_KEY_WRAP_PAD,
   AesKeyWrapKwp = CKM_AES_KEY_WRAP_KWP,
   AesKeyWrapPkcs7 = CKM_AES_KEY_WRAP_PKCS7,
   RsaPkcsTpm11 = CKM_RSA_PKCS_TPM_1_1,
   RsaPkcsOaepTpm11 = CKM_RSA_PKCS_OAEP_TPM_1_1,
   Sha1KeyGen = CKM_SHA_1_KEY_GEN,
   Sha224KeyGen = CKM_SHA224_KEY_GEN,
   Sha256KeyGen = CKM_SHA256_KEY_GEN,
   Sha384KeyGen = CKM_SHA384_KEY_GEN,
   Sha512KeyGen = CKM_SHA512_KEY_GEN,
   Sha512_224KeyGen = CKM_SHA512_224_KEY_GEN,
   Sha512_256KeyGen = CKM_SHA512_256_KEY_GEN,
   Sha512TKeyGen = CKM_SHA512_T_KEY_GEN,
   Null = CKM_NULL,
   Blake2b160 = CKM_BLAKE2B_160,
   Blake2b160Hmac = CKM_BLAKE2B_160_HMAC,
   Blake2b160HmacGeneral = CKM_BLAKE2B_160_HMAC_GENERAL,
   Blake2b160KeyDerive = CKM_BLAKE2B_160_KEY_DERIVE,
   Blake2b160KeyGen = CKM_BLAKE2B_160_KEY_GEN,
   Blake2b256 = CKM_BLAKE2B_256,
   Blake2b256Hmac = CKM_BLAKE2B_256_HMAC,
   Blake2b256HmacGeneral = CKM_BLAKE2B_256_HMAC_GENERAL,
   Blake2b256KeyDerive = CKM_BLAKE2B_256_KEY_DERIVE,
   Blake2b256KeyGen = CKM_BLAKE2B_256_KEY_GEN,
   Blake2b384 = CKM_BLAKE2B_384,
   Blake2b384Hmac = CKM_BLAKE2B_384_HMAC,
   Blake2b384HmacGeneral = CKM_BLAKE2B_384_HMAC_GENERAL,
   Blake2b384KeyDerive = CKM_BLAKE2B_384_KEY_DERIVE,
   Blake2b384KeyGen = CKM_BLAKE2B_384_KEY_GEN,
   Blake2b512 = CKM_BLAKE2B_512,
   Blake2b512Hmac = CKM_BLAKE2B_512_HMAC,
   Blake2b512HmacGeneral = CKM_BLAKE2B_512_HMAC_GENERAL,
   Blake2b512KeyDerive = CKM_BLAKE2B_512_KEY_DERIVE,
   Blake2b512KeyGen = CKM_BLAKE2B_512_KEY_GEN,
   Salsa20 = CKM_SALSA20,
   Chacha20Poly1305 = CKM_CHACHA20_POLY1305,
   Salsa20Poly1305 = CKM_SALSA20_POLY1305,
   X3dhInitialize = CKM_X3DH_INITIALIZE,
   X3dhRespond = CKM_X3DH_RESPOND,
   X2ratchetInitialize = CKM_X2RATCHET_INITIALIZE,
   X2ratchetRespond = CKM_X2RATCHET_RESPOND,
   X2ratchetEncrypt = CKM_X2RATCHET_ENCRYPT,
   X2ratchetDecrypt = CKM_X2RATCHET_DECRYPT,
   Xeddsa = CKM_XEDDSA,
   HkdfDerive = CKM_HKDF_DERIVE,
   HkdfData = CKM_HKDF_DATA,
   HkdfKeyGen = CKM_HKDF_KEY_GEN,
   Salsa20_KeyGen = CKM_SALSA20_KEY_GEN,
   EcdsaSha3_224 = CKM_ECDSA_SHA3_224,
   EcdsaSha3_256 = CKM_ECDSA_SHA3_256,
   EcdsaSha3_384 = CKM_ECDSA_SHA3_384,
   EcdsaSha3_512 = CKM_ECDSA_SHA3_512,
   EcEdwardsKeyPairGen = CKM_EC_EDWARDS_KEY_PAIR_GEN,
   EcMontgomeryKeyPairGen = CKM_EC_MONTGOMERY_KEY_PAIR_GEN,
   Eddsa = CKM_EDDSA,
   Sp800_108CounterKdf = CKM_SP800_108_COUNTER_KDF,
   Sp800_108FeedbackKdf = CKM_SP800_108_FEEDBACK_KDF,
   Sp800_108DoublePipelineKdf = CKM_SP800_108_DOUBLE_PIPELINE_KDF,
   Ike2PrfPlusDerive = CKM_IKE2_PRF_PLUS_DERIVE,
   IkePrfDerive = CKM_IKE_PRF_DERIVE,
   Ike1PrfDerive = CKM_IKE1_PRF_DERIVE,
   Ike1ExtendedDerive = CKM_IKE1_EXTENDED_DERIVE,
   HssKeyPairGen = CKM_HSS_KEY_PAIR_GEN,
   Hss = CKM_HSS,
   XmssKeyPairGen = CKM_XMSS_KEY_PAIR_GEN,
   XmssmtKeyPairGen = CKM_XMSSMT_KEY_PAIR_GEN,
   Xmss = CKM_XMSS,
   Xmssmt = CKM_XMSSMT,
   EcdhXAesKeyWrap = CKM_ECDH_X_AES_KEY_WRAP,
   EcdhCofAesKeyWrap = CKM_ECDH_COF_AES_KEY_WRAP,
   PubKeyFromPrivKey = CKM_PUB_KEY_FROM_PRIV_KEY,
   MlKemKeyPairGen = CKM_ML_KEM_KEY_PAIR_GEN,
   MlKem = CKM_ML_KEM,
   MlDsaKeyPairGen = CKM_ML_DSA_KEY_PAIR_GEN,
   MlDsa = CKM_ML_DSA,
   HashMlDsa = CKM_HASH_ML_DSA,
   HashMlDsaSha224 = CKM_HASH_ML_DSA_SHA224,
   HashMlDsaSha256 = CKM_HASH_ML_DSA_SHA256,
   HashMlDsaSha384 = CKM_HASH_ML_DSA_SHA384,
   HashMlDsaSha512 = CKM_HASH_ML_DSA_SHA512,
   HashMlDsaSha3_224 = CKM_HASH_ML_DSA_SHA3_224,
   HashMlDsaSha3_256 = CKM_HASH_ML_DSA_SHA3_256,
   HashMlDsaSha3_384 = CKM_HASH_ML_DSA_SHA3_384,
   HashMlDsaSha3_512 = CKM_HASH_ML_DSA_SHA3_512,
   HashMlDsaShake128 = CKM_HASH_ML_DSA_SHAKE128,
   HashMlDsaShake256 = CKM_HASH_ML_DSA_SHAKE256,
   SlhDsaKeyPairGen = CKM_SLH_DSA_KEY_PAIR_GEN,
   SlhDsa = CKM_SLH_DSA,
   HashSlhDsa = CKM_HASH_SLH_DSA,
   HashSlhDsaSha224 = CKM_HASH_SLH_DSA_SHA224,
   HashSlhDsaSha256 = CKM_HASH_SLH_DSA_SHA256,
   HashSlhDsaSha384 = CKM_HASH_SLH_DSA_SHA384,
   HashSlhDsaSha512 = CKM_HASH_SLH_DSA_SHA512,
   HashSlhDsaSha3_224 = CKM_HASH_SLH_DSA_SHA3_224,
   HashSlhDsaSha3_256 = CKM_HASH_SLH_DSA_SHA3_256,
   HashSlhDsaSha3_384 = CKM_HASH_SLH_DSA_SHA3_384,
   HashSlhDsaSha3_512 = CKM_HASH_SLH_DSA_SHA3_512,
   HashSlhDsaShake128 = CKM_HASH_SLH_DSA_SHAKE128,
   HashSlhDsaShake256 = CKM_HASH_SLH_DSA_SHAKE256,
   Tls12ExtendedMasterKeyDerive = CKM_TLS12_EXTENDED_MASTER_KEY_DERIVE,
   Tls12ExtendedMasterKeyDeriveDh = CKM_TLS12_EXTENDED_MASTER_KEY_DERIVE_DH,
   VendorDefined = CKM_VENDOR_DEFINED,
};

enum class Notification : CK_NOTIFICATION {
   Surrender = CKN_SURRENDER,
   OtpChanged = CKN_OTP_CHANGED,
};

enum class ObjectClass : CK_OBJECT_CLASS {
   Data = CKO_DATA,
   Certificate = CKO_CERTIFICATE,
   PublicKey = CKO_PUBLIC_KEY,
   PrivateKey = CKO_PRIVATE_KEY,
   SecretKey = CKO_SECRET_KEY,
   HwFeature = CKO_HW_FEATURE,
   DomainParameters = CKO_DOMAIN_PARAMETERS,
   Mechanism = CKO_MECHANISM,
   OtpKey = CKO_OTP_KEY,
   Profile = CKO_PROFILE,
   Validation = CKO_VALIDATION,
   Trust = CKO_TRUST,
   VendorDefined = CKO_VENDOR_DEFINED,
};

enum class ProfileId : CK_PROFILE_ID {
   InvalidId = CKP_INVALID_ID,
   BaselineProvider = CKP_BASELINE_PROVIDER,
   ExtendedProvider = CKP_EXTENDED_PROVIDER,
   AuthenticationToken = CKP_AUTHENTICATION_TOKEN,
   PublicCertificatesToken = CKP_PUBLIC_CERTIFICATES_TOKEN,
   CompleteProvider = CKP_COMPLETE_PROVIDER,
   HkdfTlsToken = CKP_HKDF_TLS_TOKEN,
   VendorDefined = CKP_VENDOR_DEFINED,
};

enum class PseudoRandom : CK_PKCS5_PBKD2_PSEUDO_RANDOM_FUNCTION_TYPE {
   Pkcs5Pbkd2HmacSha1 = CKP_PKCS5_PBKD2_HMAC_SHA1,
   Pkcs5Pbkd2HmacGostr3411 = CKP_PKCS5_PBKD2_HMAC_GOSTR3411,
   Pkcs5Pbkd2HmacSha224 = CKP_PKCS5_PBKD2_HMAC_SHA224,
   Pkcs5Pbkd2HmacSha256 = CKP_PKCS5_PBKD2_HMAC_SHA256,
   Pkcs5Pbkd2HmacSha384 = CKP_PKCS5_PBKD2_HMAC_SHA384,
   Pkcs5Pbkd2HmacSha512 = CKP_PKCS5_PBKD2_HMAC_SHA512,
   Pkcs5Pbkd2HmacSha512224 = CKP_PKCS5_PBKD2_HMAC_SHA512_224,
   Pkcs5Pbkd2HmacSha512256 = CKP_PKCS5_PBKD2_HMAC_SHA512_256,
};

enum class MlDsaParameterSet : CK_ML_DSA_PARAMETER_SET_TYPE {
   MlDsa44 = CKP_ML_DSA_44,
   MlDsa65 = CKP_ML_DSA_65,
   MlDsa87 = CKP_ML_DSA_87,
};

enum class SlhDsaParameterSet : CK_SLH_DSA_PARAMETER_SET_TYPE {
   SlhDsaSha2_128s = CKP_SLH_DSA_SHA2_128S,
   SlhDsaShake128s = CKP_SLH_DSA_SHAKE_128S,
   SlhDsaSha2_128f = CKP_SLH_DSA_SHA2_128F,
   SlhDsaShake128f = CKP_SLH_DSA_SHAKE_128F,
   SlhDsaSha2_192s = CKP_SLH_DSA_SHA2_192S,
   SlhDsaShake192s = CKP_SLH_DSA_SHAKE_192S,
   SlhDsaSha2_192f = CKP_SLH_DSA_SHA2_192F,
   SlhDsaShake192f = CKP_SLH_DSA_SHAKE_192F,
   SlhDsaSha2_256s = CKP_SLH_DSA_SHA2_256S,
   SlhDsaShake256s = CKP_SLH_DSA_SHAKE_256S,
   SlhDsaSha2_256f = CKP_SLH_DSA_SHA2_256F,
   SlhDsaShake256f = CKP_SLH_DSA_SHAKE_256F,
};

enum class MlKemParameterSet : CK_ML_KEM_PARAMETER_SET_TYPE {
   MlKem512 = CKP_ML_KEM_512,
   MlKem768 = CKP_ML_KEM_768,
   MlKem1024 = CKP_ML_KEM_1024,
};

enum class SessionState : CK_STATE {
   RoPublicSession = CKS_RO_PUBLIC_SESSION,
   RoUserFunctions = CKS_RO_USER_FUNCTIONS,
   RwPublicSession = CKS_RW_PUBLIC_SESSION,
   RwUserFunctions = CKS_RW_USER_FUNCTIONS,
   RwSoFunctions = CKS_RW_SO_FUNCTIONS,
};

enum class SessionValidationFlagsType : CK_SESSION_VALIDATION_FLAGS_TYPE {
   LastValidationOk = CKS_LAST_VALIDATION_OK,
};

enum class Trust : CK_TRUST {
   TrustUnknown = CKT_TRUST_UNKNOWN,
   Trusted = CKT_TRUSTED,
   TrustAnchor = CKT_TRUST_ANCHOR,
   NotTrusted = CKT_NOT_TRUSTED,
   TrustMustVerifyTrust = CKT_TRUST_MUST_VERIFY_TRUST,
};

enum class ReturnValue : CK_RV {
   OK = CKR_OK,
   Cancel = CKR_CANCEL,
   HostMemory = CKR_HOST_MEMORY,
   SlotIdInvalid = CKR_SLOT_ID_INVALID,
   GeneralError = CKR_GENERAL_ERROR,
   FunctionFailed = CKR_FUNCTION_FAILED,
   ArgumentsBad = CKR_ARGUMENTS_BAD,
   NoEvent = CKR_NO_EVENT,
   NeedToCreateThreads = CKR_NEED_TO_CREATE_THREADS,
   CantLock = CKR_CANT_LOCK,
   AttributeReadOnly = CKR_ATTRIBUTE_READ_ONLY,
   AttributeSensitive = CKR_ATTRIBUTE_SENSITIVE,
   AttributeTypeInvalid = CKR_ATTRIBUTE_TYPE_INVALID,
   AttributeValueInvalid = CKR_ATTRIBUTE_VALUE_INVALID,
   ActionProhibited = CKR_ACTION_PROHIBITED,
   DataInvalid = CKR_DATA_INVALID,
   DataLenRange = CKR_DATA_LEN_RANGE,
   DeviceError = CKR_DEVICE_ERROR,
   DeviceMemory = CKR_DEVICE_MEMORY,
   DeviceRemoved = CKR_DEVICE_REMOVED,
   EncryptedDataInvalid = CKR_ENCRYPTED_DATA_INVALID,
   EncryptedDataLenRange = CKR_ENCRYPTED_DATA_LEN_RANGE,
   AeadDecryptFailed = CKR_AEAD_DECRYPT_FAILED,
   FunctionCanceled = CKR_FUNCTION_CANCELED,
   FunctionNotParallel = CKR_FUNCTION_NOT_PARALLEL,
   FunctionNotSupported = CKR_FUNCTION_NOT_SUPPORTED,
   KeyHandleInvalid = CKR_KEY_HANDLE_INVALID,
   KeySizeRange = CKR_KEY_SIZE_RANGE,
   KeyTypeInconsistent = CKR_KEY_TYPE_INCONSISTENT,
   KeyNotNeeded = CKR_KEY_NOT_NEEDED,
   KeyChanged = CKR_KEY_CHANGED,
   KeyNeeded = CKR_KEY_NEEDED,
   KeyIndigestible = CKR_KEY_INDIGESTIBLE,
   KeyFunctionNotPermitted = CKR_KEY_FUNCTION_NOT_PERMITTED,
   KeyNotWrappable = CKR_KEY_NOT_WRAPPABLE,
   KeyUnextractable = CKR_KEY_UNEXTRACTABLE,
   MechanismInvalid = CKR_MECHANISM_INVALID,
   MechanismParamInvalid = CKR_MECHANISM_PARAM_INVALID,
   ObjectHandleInvalid = CKR_OBJECT_HANDLE_INVALID,
   OperationActive = CKR_OPERATION_ACTIVE,
   OperationNotInitialized = CKR_OPERATION_NOT_INITIALIZED,
   PinIncorrect = CKR_PIN_INCORRECT,
   PinInvalid = CKR_PIN_INVALID,
   PinLenRange = CKR_PIN_LEN_RANGE,
   PinExpired = CKR_PIN_EXPIRED,
   PinLocked = CKR_PIN_LOCKED,
   SessionClosed = CKR_SESSION_CLOSED,
   SessionCount = CKR_SESSION_COUNT,
   SessionHandleInvalid = CKR_SESSION_HANDLE_INVALID,
   SessionParallelNotSupported = CKR_SESSION_PARALLEL_NOT_SUPPORTED,
   SessionReadOnly = CKR_SESSION_READ_ONLY,
   SessionExists = CKR_SESSION_EXISTS,
   SessionReadOnlyExists = CKR_SESSION_READ_ONLY_EXISTS,
   SessionReadWriteSoExists = CKR_SESSION_READ_WRITE_SO_EXISTS,
   SignatureInvalid = CKR_SIGNATURE_INVALID,
   SignatureLenRange = CKR_SIGNATURE_LEN_RANGE,
   TemplateIncomplete = CKR_TEMPLATE_INCOMPLETE,
   TemplateInconsistent = CKR_TEMPLATE_INCONSISTENT,
   TokenNotPresent = CKR_TOKEN_NOT_PRESENT,
   TokenNotRecognized = CKR_TOKEN_NOT_RECOGNIZED,
   TokenWriteProtected = CKR_TOKEN_WRITE_PROTECTED,
   UnwrappingKeyHandleInvalid = CKR_UNWRAPPING_KEY_HANDLE_INVALID,
   UnwrappingKeySizeRange = CKR_UNWRAPPING_KEY_SIZE_RANGE,
   UnwrappingKeyTypeInconsistent = CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT,
   UserAlreadyLoggedIn = CKR_USER_ALREADY_LOGGED_IN,
   UserNotLoggedIn = CKR_USER_NOT_LOGGED_IN,
   UserPinNotInitialized = CKR_USER_PIN_NOT_INITIALIZED,
   UserTypeInvalid = CKR_USER_TYPE_INVALID,
   UserAnotherAlreadyLoggedIn = CKR_USER_ANOTHER_ALREADY_LOGGED_IN,
   UserTooManyTypes = CKR_USER_TOO_MANY_TYPES,
   WrappedKeyInvalid = CKR_WRAPPED_KEY_INVALID,
   WrappedKeyLenRange = CKR_WRAPPED_KEY_LEN_RANGE,
   WrappingKeyHandleInvalid = CKR_WRAPPING_KEY_HANDLE_INVALID,
   WrappingKeySizeRange = CKR_WRAPPING_KEY_SIZE_RANGE,
   WrappingKeyTypeInconsistent = CKR_WRAPPING_KEY_TYPE_INCONSISTENT,
   RandomSeedNotSupported = CKR_RANDOM_SEED_NOT_SUPPORTED,
   RandomNoRng = CKR_RANDOM_NO_RNG,
   DomainParamsInvalid = CKR_DOMAIN_PARAMS_INVALID,
   CurveNotSupported = CKR_CURVE_NOT_SUPPORTED,
   BufferTooSmall = CKR_BUFFER_TOO_SMALL,
   SavedStateInvalid = CKR_SAVED_STATE_INVALID,
   InformationSensitive = CKR_INFORMATION_SENSITIVE,
   StateUnsaveable = CKR_STATE_UNSAVEABLE,
   CryptokiNotInitialized = CKR_CRYPTOKI_NOT_INITIALIZED,
   CryptokiAlreadyInitialized = CKR_CRYPTOKI_ALREADY_INITIALIZED,
   MutexBad = CKR_MUTEX_BAD,
   MutexNotLocked = CKR_MUTEX_NOT_LOCKED,
   NewPinMode = CKR_NEW_PIN_MODE,
   NextOtp = CKR_NEXT_OTP,
   ExceededMaxIterations = CKR_EXCEEDED_MAX_ITERATIONS,
   FipsSelfTestFailed = CKR_FIPS_SELF_TEST_FAILED,
   LibraryLoadFailed = CKR_LIBRARY_LOAD_FAILED,
   PinTooWeak = CKR_PIN_TOO_WEAK,
   PublicKeyInvalid = CKR_PUBLIC_KEY_INVALID,
   FunctionRejected = CKR_FUNCTION_REJECTED,
   TokenResourceExceeded = CKR_TOKEN_RESOURCE_EXCEEDED,
   OperationCancelFailed = CKR_OPERATION_CANCEL_FAILED,
   KeyExhausted = CKR_KEY_EXHAUSTED,
   Pending = CKR_PENDING,
   SessionAsyncNotSupported = CKR_SESSION_ASYNC_NOT_SUPPORTED,
   SeedRandomRequired = CKR_SEED_RANDOM_REQUIRED,
   OperationNotValidated = CKR_OPERATION_NOT_VALIDATED,
   TokenNotInitialized = CKR_TOKEN_NOT_INITIALIZED,
   ParameterSetNotSupported = CKR_PARAMETER_SET_NOT_SUPPORTED,
   VendorDefined = CKR_VENDOR_DEFINED,
};

enum class UserType : CK_USER_TYPE {
   SO = CKU_SO,
   User = CKU_USER,
   ContextSpecific = CKU_CONTEXT_SPECIFIC,
};

enum class ValidationAuthorityType : CK_VALIDATION_AUTHORITY_TYPE {
   AuthorityTypeUnspecified = CKV_AUTHORITY_TYPE_UNSPECIFIED,
   AuthorityTypeNistCmvp = CKV_AUTHORITY_TYPE_NIST_CMVP,
   AuthorityTypeCommonCriteria = CKV_AUTHORITY_TYPE_COMMON_CRITERIA,
};

enum class ValidationType : CK_VALIDATION_TYPE {
   TypeUnspecified = CKV_TYPE_UNSPECIFIED,
   TypeSoftware = CKV_TYPE_SOFTWARE,
   TypeHardware = CKV_TYPE_HARDWARE,
   TypeFirmware = CKV_TYPE_FIRMWARE,
   TypeHybrid = CKV_TYPE_HYBRID,
};

enum class PublicPointEncoding : uint32_t { Raw, Der };

// NOLINTEND(*-enum-size)

using FunctionList = CK_FUNCTION_LIST;
using FunctionListPtr = CK_FUNCTION_LIST_PTR;
using FunctionList30 = CK_FUNCTION_LIST_3_0;
using FunctionList32 = CK_FUNCTION_LIST_3_2;
using Interface = CK_INTERFACE;
using VoidPtr = CK_VOID_PTR;
using C_InitializeArgs = CK_C_INITIALIZE_ARGS;
using CreateMutex = CK_CREATEMUTEX;
using DestroyMutex = CK_DESTROYMUTEX;
using LockMutex = CK_LOCKMUTEX;
using UnlockMutex = CK_UNLOCKMUTEX;
using Flags = CK_FLAGS;
using Info = CK_INFO;
using Version = CK_VERSION;
using Bbool = CK_BBOOL;
using SlotId = CK_SLOT_ID;
using Ulong = CK_ULONG;
using SlotInfo = CK_SLOT_INFO;
using TokenInfo = CK_TOKEN_INFO;
using Mechanism = CK_MECHANISM;
using MechanismInfo = CK_MECHANISM_INFO;
using Utf8Char = CK_UTF8CHAR;
using Notify = CK_NOTIFY;
using SessionHandle = CK_SESSION_HANDLE;
using SessionInfo = CK_SESSION_INFO;
using Attribute = CK_ATTRIBUTE;
using ObjectHandle = CK_OBJECT_HANDLE;
using Byte = CK_BYTE;
using RsaPkcsOaepParams = CK_RSA_PKCS_OAEP_PARAMS;
using RsaPkcsPssParams = CK_RSA_PKCS_PSS_PARAMS;
using Ecdh1DeriveParams = CK_ECDH1_DERIVE_PARAMS;
using Date = CK_DATE;
using AsyncData = CK_ASYNC_DATA;

// NOLINTNEXTLINE(*-avoid-non-const-global-variables) TODO can this be made const?
BOTAN_PUBLIC_API(2, 0) extern ReturnValue* ThrowException;

const Bbool True = CK_TRUE;
const Bbool False = CK_FALSE;

inline Flags flags(Flag flags) {
   return static_cast<Flags>(flags);
}

class Slot;

/**
* Initializes a token
* @param slot The slot with the attached token that should be initialized
* @param label The token label
* @param so_pin PIN of the security officer. Will be set if the token is uninitialized other this has to be the current SO_PIN
* @param pin The user PIN that will be set
*/
BOTAN_PUBLIC_API(2, 0)
void initialize_token(Slot& slot, std::string_view label, const secure_string& so_pin, const secure_string& pin);

/**
* Change PIN with old PIN to new PIN
* @param slot The slot with the attached token
* @param old_pin The old user PIN
* @param new_pin The new user PIN
*/

BOTAN_PUBLIC_API(2, 0) void change_pin(Slot& slot, const secure_string& old_pin, const secure_string& new_pin);

/**
* Change SO_PIN with old SO_PIN to new SO_PIN
* @param slot The slot with the attached token
* @param old_so_pin The old SO_PIN
* @param new_so_pin The new SO_PIN
*/
BOTAN_PUBLIC_API(2, 0) void change_so_pin(Slot& slot, const secure_string& old_so_pin, const secure_string& new_so_pin);

/**
* Sets user PIN with SO_PIN
* @param slot The slot with the attached token
* @param so_pin PIN of the security officer
* @param pin The user PIN that should be set
*/
BOTAN_PUBLIC_API(2, 0) void set_pin(Slot& slot, const secure_string& so_pin, const secure_string& pin);

/**
 * @brief Wraps a PKCS #11 Interface object.
 *
 * This class provides an interface to access PKCS #11 functions of various versions.
 * For example func_3_0() returns the PKCS #11 v3.0 function list for a loaded interface.
 * Only the official "PKCS 11" named interfaces are supported.
 */
class BOTAN_PUBLIC_API(3, 11) InterfaceWrapper final {
   private:
      Interface m_p11_interface;

   public:
      /// Basic constructor using an interface.
      explicit InterfaceWrapper(Interface p11_interface);

      InterfaceWrapper(const InterfaceWrapper&) = default;
      InterfaceWrapper& operator=(const InterfaceWrapper&) = default;
      InterfaceWrapper(InterfaceWrapper&&) = default;
      InterfaceWrapper& operator=(InterfaceWrapper&&) = default;
      ~InterfaceWrapper() = default;

      /// Access the underlying interface object
      const Interface& raw_interface() const { return m_p11_interface; }

      /// Access the version of the interface
      Version version() const;

      /// Access the name of the interface
      std::span<const Utf8Char> name() const;

      /// Access a function list that contains all methods since PKCS #11 v.2.40
      const FunctionList& func_2_40() const;

      /// Access a function list that contains all methods since PKCS #11 v.3.0
      const FunctionList30& func_3_0() const;

      /// Access a function list that contains all methods since PKCS #11 v.3.2
      const FunctionList32& func_3_2() const;

      /// Find the latest supported "PKCS 11" interface. Fork safe interfaces
      /// are preferred over non fork safe ones of the same version.
      static InterfaceWrapper latest_p11_interface(Dynamically_Loaded_Library& library);

      /**
       * Returns an immortal pointer to the Utf8Char string "PKCS 11".
       * Used to define an interface object.
       *
       * @warning Unfortunately, the interface object requires a non constant
       * pointer. However, this string MUST NOT be modified!
       */
      static Utf8Char* p11_interface_name_ptr();
};

/// Provides access to all PKCS #11 functions
class BOTAN_PUBLIC_API(2, 0) LowLevel {
   public:
      /// @param ptr the function list pointer to use. Can be retrieved via `LowLevel::C_GetFunctionList`
      BOTAN_DEPRECATED("Use LowLevel(InterfaceWrapper::latest_p11_interface(module.library()))")
      explicit LowLevel(FunctionList* ptr);

      explicit LowLevel(InterfaceWrapper interface_wrapper);

      /****************************** General purpose functions ******************************/

      /**
      * C_Initialize initializes the Cryptoki library.
      * @param init_args if this is not nullptr, it gets cast to (`C_InitializeArgs`) and dereferenced
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CantLock \li CryptokiAlreadyInitialized
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li NeedToCreateThreads \li OK
      * @return true on success, false otherwise
      */
      bool C_Initialize(const void* init_args, ReturnValue* return_value = ThrowException) const;

      /**
      * C_Finalize indicates that an application is done with the Cryptoki library.
      * @param reserved reserved.  Should be nullptr
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      * @return true on success, false otherwise
      */
      bool C_Finalize(void* reserved, ReturnValue* return_value = ThrowException) const;

      /**
      * C_GetInfo returns general information about Cryptoki.
      * @param info_ptr location that receives information
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      * @return true on success, false otherwise
      */
      bool C_GetInfo(Info* info_ptr, ReturnValue* return_value = ThrowException) const;

      /**
      * C_GetFunctionList returns the function list.
      * @param pkcs11_module The PKCS #11 module
      * @param function_list_ptr_ptr receives pointer to function list
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK
      * @return true on success, false otherwise
      */
      static bool C_GetFunctionList(const Dynamically_Loaded_Library& pkcs11_module,
                                    FunctionList** function_list_ptr_ptr,
                                    ReturnValue* return_value = ThrowException);

      /**
       * C_GetInterfaceList is used to obtain a list of interfaces supported by
       * a Cryptoki library. count_ptr points to the location that receives the
       * number of interfaces. There are two ways for an application to call
       * C_GetInterfaceList:
       *  1. If interface_list_ptr is nullptr, then all that C_GetInterfaceList
       *     does is return (in *count_ptr) the number of interfaces, without
       *     actually returning a list of interfaces. The contents of *count_ptr
       *     on entry to C_GetInterfaceList have no meaning in this case, and the
       *     call returns the value CKR_OK.
       *  2. If pIntrerfaceList is not nullptr, then *count_ptr MUST contain the
       *     size (in terms of CK_INTERFACE elements) of the buffer pointed to
       *     by interface_list_ptr. If that buffer is large enough to  hold the
       *     list of interfaces, then the list is returned in it, and CKR_OK is
       *     returned. If not, then the call to C_GetInterfaceList returns the
       *     value CKR_BUFFER_TOO_SMALL. In either case, the value *count_ptr is
       *     set to hold the number of interfaces.
       *
       * Because C_GetInterfaceList does not allocate any space of its own, an
       * application will often call C_GetInterfaceList twice. However, this
       * behavior is by no means required. C_GetInterfaceList obtains
       * (in *pFunctionList of each interface) a pointer to the Cryptoki
       * library’s list of function pointers. The pointer thus obtained may
       * point into memory which is owned by the Cryptoki library, and which
       * may or may not be writable. Whether or not this is the case, no attempt
       * should be made to write to this memory. The same caveat applies to
       * the interface names returned.
       *
       * @param pkcs11_module The PKCS #11 module
       * @param interface_list_ptr returned interfaces
       * @param count_ptr number of interfaces returned
       * @param return_value default value (`ThrowException`): throw exception on error.
       * @return true on success, false otherwise
       */
      static bool C_GetInterfaceList(const Dynamically_Loaded_Library& pkcs11_module,
                                     Interface* interface_list_ptr,
                                     Ulong* count_ptr,
                                     ReturnValue* return_value = ThrowException);

      /**
       * C_GetInterface is used to obtain an interface supported by a Cryptoki
       * library. pInterfaceName specifies  the name of the interface, pVersion
       * specifies the interface version, ppInterface points to the location
       * that  receives the interface, flags specifies the required interface
       * flags. There are multiple ways for an application to specify a
       * particular interface when calling C_GetInterface:
       * 1. If pInterfaceName is not nullptr, the name of the interface
       *    returned must match. If  pInterfaceName is nullptr, the cryptoki
       *    library can return a default interface of its choice
       * 2. If pVersion is not nullptr, the version of the interface returned
       *    must match. If pVersion is  nullptr, the cryptoki library can
       *    return an interface of any version
       * 3. If flags is non-zero, the interface returned must match all of the
       *    supplied flag values (but may include  additional flags not
       *    specified). If flags is 0, the cryptoki library can return an
       *    interface with any flags
       *
       * @param pkcs11_module The PKCS #11 module
       * @param interface_name_ptr name of the interface
       * @param version_ptr version of the interface
       * @param interface_ptr_ptr returned interface
       * @param flags flags controlling the semantics of the interface
       * @param return_value default value (`ThrowException`): throw exception on error.
       * @return true on success, false otherwise
       */
      static bool C_GetInterface(const Dynamically_Loaded_Library& pkcs11_module,
                                 const Utf8Char* interface_name_ptr,
                                 const Version* version_ptr,
                                 Interface* interface_ptr_ptr,
                                 Flags flags,
                                 ReturnValue* return_value = ThrowException);

      /****************************** Slot and token management functions ******************************/

      /**
      * C_GetSlotList obtains a list of slots in the system.
      * @param token_present only slots with tokens
      * @param slot_list_ptr receives array of slot IDs
      * @param count_ptr receives number of slots
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK
      * @return true on success, false otherwise
      */
      bool C_GetSlotList(Bbool token_present,
                         SlotId* slot_list_ptr,
                         Ulong* count_ptr,
                         ReturnValue* return_value = ThrowException) const;

      /**
      * C_GetSlotList obtains a list of slots in the system.
      * @param token_present only slots with tokens
      * @param slot_ids receives vector of slot IDs
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK
      * @return true on success, false otherwise
      */
      bool C_GetSlotList(bool token_present,
                         std::vector<SlotId>& slot_ids,
                         ReturnValue* return_value = ThrowException) const;

      /**
      * C_GetSlotInfo obtains information about a particular slot in the system.
      * @param slot_id the ID of the slot
      * @param info_ptr receives the slot information
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DeviceError
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li SlotIdInvalid
      * @return true on success, false otherwise
      */
      bool C_GetSlotInfo(SlotId slot_id, SlotInfo* info_ptr, ReturnValue* return_value = ThrowException) const;

      /**
      * C_GetTokenInfo obtains information about a particular token in the system.
      * @param slot_id ID of the token's slot
      * @param info_ptr receives the token information
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li SlotIdInvalid
      *     \li TokenNotPresent \li TokenNotRecognized \li ArgumentsBad
      * @return true on success, false otherwise
      */
      bool C_GetTokenInfo(SlotId slot_id, TokenInfo* info_ptr, ReturnValue* return_value = ThrowException) const;

      /**
      * C_WaitForSlotEvent waits for a slot event (token insertion, removal, etc.) to occur.
      * @param flags blocking/nonblocking flag
      * @param slot_ptr location that receives the slot ID
      * @param reserved reserved. Should be nullptr
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li FunctionFailed
      *     \li GeneralError \li HostMemory \li NoEvent
      *     \li OK
      * @return true on success, false otherwise
      */
      bool C_WaitForSlotEvent(Flags flags,
                              SlotId* slot_ptr,
                              void* reserved,
                              ReturnValue* return_value = ThrowException) const;

      /**
      * C_GetMechanismList obtains a list of mechanism types supported by a token.
      * @param slot_id ID of token's slot
      * @param mechanism_list_ptr gets mech. array
      * @param count_ptr gets # of mechs.
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li BufferTooSmall \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li SlotIdInvalid \li TokenNotPresent \li TokenNotRecognized
      *     \li ArgumentsBad
      * @return true on success, false otherwise
      */
      bool C_GetMechanismList(SlotId slot_id,
                              MechanismType* mechanism_list_ptr,
                              Ulong* count_ptr,
                              ReturnValue* return_value = ThrowException) const;

      /**
      * C_GetMechanismList obtains a list of mechanism types supported by a token.
      * @param slot_id ID of token's slot
      * @param mechanisms receives vector of supported mechanisms
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li BufferTooSmall \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li SlotIdInvalid \li TokenNotPresent \li TokenNotRecognized
      *     \li ArgumentsBad
      * @return true on success, false otherwise
      */
      bool C_GetMechanismList(SlotId slot_id,
                              std::vector<MechanismType>& mechanisms,
                              ReturnValue* return_value = ThrowException) const;

      /**
      * C_GetMechanismInfo obtains information about a particular mechanism possibly supported by a token.
      * @param slot_id ID of the token's slot
      * @param type type of mechanism
      * @param info_ptr receives mechanism info
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionFailed \li GeneralError
      *     \li HostMemory \li MechanismInvalid \li OK
      *     \li SlotIdInvalid \li TokenNotPresent \li TokenNotRecognized
      *     \li ArgumentsBad
      * @return true on success, false otherwise
      */
      bool C_GetMechanismInfo(SlotId slot_id,
                              MechanismType type,
                              MechanismInfo* info_ptr,
                              ReturnValue* return_value = ThrowException) const;

      /**
      * C_InitToken initializes a token.
      * @param slot_id ID of the token's slot
      * @param so_pin_ptr the SO's initial PIN
      * @param so_pin_len length in bytes of the SO_PIN
      * @param label_ptr 32-byte token label (blank padded)
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li PinIncorrect \li PinLocked \li SessionExists
      *     \li SlotIdInvalid \li TokenNotPresent \li TokenNotRecognized
      *     \li TokenWriteProtected \li ArgumentsBad
      * @return true on success, false otherwise
      */
      bool C_InitToken(SlotId slot_id,
                       const Utf8Char* so_pin_ptr,
                       Ulong so_pin_len,
                       const Utf8Char* label_ptr,
                       ReturnValue* return_value = ThrowException) const;

      /**
      * C_InitToken initializes a token.
      * @param slot_id ID of the token's slot
      * @param so_pin the SO's initial PIN
      * @param label token label (at max 32 bytes long)
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li PinIncorrect \li PinLocked \li SessionExists
      *     \li SlotIdInvalid \li TokenNotPresent \li TokenNotRecognized
      *     \li TokenWriteProtected \li ArgumentsBad
      * @return true on success, false otherwise
      */
      template <typename TAlloc>
      bool C_InitToken(SlotId slot_id,
                       const std::vector<uint8_t, TAlloc>& so_pin,
                       std::string_view label,
                       ReturnValue* return_value = ThrowException) const {
         std::string padded_label(label);
         if(label.size() < 32) {
            padded_label.insert(padded_label.end(), 32 - label.size(), ' ');
         }

         return C_InitToken(slot_id,
                            reinterpret_cast<Utf8Char*>(const_cast<uint8_t*>(so_pin.data())),
                            static_cast<Ulong>(so_pin.size()),
                            reinterpret_cast<Utf8Char*>(const_cast<char*>(padded_label.c_str())),
                            return_value);
      }

      /**
      * C_InitPIN initializes the normal user's PIN.
      * @param session the session's handle
      * @param pin_ptr the normal user's PIN
      * @param pin_len length in bytes of the PIN
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li PinInvalid \li PinLenRange \li SessionClosed
      *     \li SessionReadOnly \li SessionHandleInvalid \li TokenWriteProtected
      *     \li UserNotLoggedIn \li ArgumentsBad
      * @return true on success, false otherwise
      */
      bool C_InitPIN(SessionHandle session,
                     const Utf8Char* pin_ptr,
                     Ulong pin_len,
                     ReturnValue* return_value = ThrowException) const;

      /**
      * C_InitPIN initializes the normal user's PIN.
      * @param session the session's handle
      * @param pin the normal user's PIN
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li PinInvalid \li PinLenRange \li SessionClosed
      *     \li SessionReadOnly \li SessionHandleInvalid \li TokenWriteProtected
      *     \li UserNotLoggedIn \li ArgumentsBad
      * @return true on success, false otherwise
      */
      template <typename TAlloc>
      bool C_InitPIN(SessionHandle session,
                     const std::vector<uint8_t, TAlloc>& pin,
                     ReturnValue* return_value = ThrowException) const {
         return C_InitPIN(session,
                          reinterpret_cast<Utf8Char*>(const_cast<uint8_t*>(pin.data())),
                          static_cast<Ulong>(pin.size()),
                          return_value);
      }

      /**
      * C_SetPIN modifies the PIN of the user who is logged in.
      * @param session the session's handle
      * @param old_pin_ptr the old PIN
      * @param old_len length of the old PIN
      * @param new_pin_ptr the new PIN
      * @param new_len length of the new PIN
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li PinIncorrect \li PinInvalid \li PinLenRange
      *     \li PinLocked \li SessionClosed \li SessionHandleInvalid
      *     \li SessionReadOnly \li TokenWriteProtected \li ArgumentsBad
      * @return true on success, false otherwise
      */
      bool C_SetPIN(SessionHandle session,
                    const Utf8Char* old_pin_ptr,
                    Ulong old_len,
                    const Utf8Char* new_pin_ptr,
                    Ulong new_len,
                    ReturnValue* return_value = ThrowException) const;

      /**
      * C_SetPIN modifies the PIN of the user who is logged in.
      * @param session the session's handle
      * @param old_pin the old PIN
      * @param new_pin the new PIN
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li PinIncorrect \li PinInvalid \li PinLenRange
      *     \li PinLocked \li SessionClosed \li SessionHandleInvalid
      *     \li SessionReadOnly \li TokenWriteProtected \li ArgumentsBad
      * @return true on success, false otherwise
      */
      template <typename TAlloc>
      bool C_SetPIN(SessionHandle session,
                    const std::vector<uint8_t, TAlloc>& old_pin,
                    const std::vector<uint8_t, TAlloc>& new_pin,
                    ReturnValue* return_value = ThrowException) const {
         return C_SetPIN(session,
                         reinterpret_cast<Utf8Char*>(const_cast<uint8_t*>(old_pin.data())),
                         static_cast<Ulong>(old_pin.size()),
                         reinterpret_cast<Utf8Char*>(const_cast<uint8_t*>(new_pin.data())),
                         static_cast<Ulong>(new_pin.size()),
                         return_value);
      }

      /****************************** Session management ******************************/

      /**
      * C_OpenSession opens a session between an application and a token.
      * @param slot_id the slot's ID
      * @param flags from CK_SESSION_INFO
      * @param application passed to callback
      * @param notify callback function
      * @param session_ptr gets session handle
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li SessionCount
      *     \li SessionParallelNotSupported \li SessionReadWriteSoExists \li SlotIdInvalid
      *     \li TokenNotPresent \li TokenNotRecognized \li TokenWriteProtected
      *     \li ArgumentsBad
      * @return true on success, false otherwise
      */
      bool C_OpenSession(SlotId slot_id,
                         Flags flags,
                         void* application,
                         Notify notify,
                         SessionHandle* session_ptr,
                         ReturnValue* return_value = ThrowException) const;

      /**
      * C_CloseSession closes a session between an application and a token.
      * @param session the session's handle
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li SessionClosed
      *     \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_CloseSession(SessionHandle session, ReturnValue* return_value = ThrowException) const;

      /**
      * C_CloseAllSessions closes all sessions with a token.
      * @param slot_id the token's slot
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li SlotIdInvalid
      *     \li TokenNotPresent
      * @return true on success, false otherwise
      */
      bool C_CloseAllSessions(SlotId slot_id, ReturnValue* return_value = ThrowException) const;

      /**
      * C_GetSessionInfo obtains information about the session.
      * @param session the session's handle
      * @param info_ptr receives session info
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li SessionClosed
      *     \li SessionHandleInvalid \li ArgumentsBad
      * @return true on success, false otherwise
      */
      bool C_GetSessionInfo(SessionHandle session,
                            SessionInfo* info_ptr,
                            ReturnValue* return_value = ThrowException) const;

      /**
       * C_SessionCancel terminates active session based operations.
       *
       * @param session the session's handle
       * @param flags flags control which sessions are cancelled
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_SessionCancel(SessionHandle session, Flags flags, ReturnValue* return_value = ThrowException);

      /**
      * C_GetOperationState obtains the state of the cryptographic operation in a session.
      * @param session session's handle
      * @param operation_state_ptr gets state
      * @param operation_state_len_ptr gets state length
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li BufferTooSmall \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li OperationNotInitialized \li SessionClosed \li SessionHandleInvalid
      *     \li StateUnsaveable \li ArgumentsBad
      * @return true on success, false otherwise
      */
      bool C_GetOperationState(SessionHandle session,
                               Byte* operation_state_ptr,
                               Ulong* operation_state_len_ptr,
                               ReturnValue* return_value = ThrowException) const;

      /**
      * C_SetOperationState restores the state of the cryptographic operation in a session.
      * @param session session's handle
      * @param operation_state_ptr holds state
      * @param operation_state_len holds state length
      * @param encryption_key en/decryption key
      * @param authentication_key sign/verify key
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionFailed \li GeneralError
      *     \li HostMemory \li KeyChanged \li KeyNeeded
      *     \li KeyNotNeeded \li OK \li SavedStateInvalid
      *     \li SessionClosed \li SessionHandleInvalid \li ArgumentsBad
      * @return true on success, false otherwise
      */
      bool C_SetOperationState(SessionHandle session,
                               const Byte* operation_state_ptr,
                               Ulong operation_state_len,
                               ObjectHandle encryption_key,
                               ObjectHandle authentication_key,
                               ReturnValue* return_value = ThrowException) const;

      /**
      * C_Login logs a user into a token.
      * @param session the session's handle
      * @param user_type the user type
      * @param pin_ptr the user's PIN
      * @param pin_len the length of the PIN
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationNotInitialized \li PinIncorrect
      *     \li PinLocked \li SessionClosed \li SessionHandleInvalid
      *     \li SessionReadOnlyExists \li UserAlreadyLoggedIn \li UserAnotherAlreadyLoggedIn
      *     \li UserPinNotInitialized \li UserTooManyTypes \li UserTypeInvalid
      * @return true on success, false otherwise
      */
      bool C_Login(SessionHandle session,
                   UserType user_type,
                   const Utf8Char* pin_ptr,
                   Ulong pin_len,
                   ReturnValue* return_value = ThrowException) const;

      /**
      * C_Login logs a user into a token.
      * @param session the session's handle
      * @param user_type the user type
      * @param pin the user or security officer's PIN
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationNotInitialized \li PinIncorrect
      *     \li PinLocked \li SessionClosed \li SessionHandleInvalid
      *     \li SessionReadOnlyExists \li UserAlreadyLoggedIn \li UserAnotherAlreadyLoggedIn
      *     \li UserPinNotInitialized \li UserTooManyTypes \li UserTypeInvalid
      * @return true on success, false otherwise
      */
      template <typename TAlloc>
      bool C_Login(SessionHandle session,
                   UserType user_type,
                   const std::vector<uint8_t, TAlloc>& pin,
                   ReturnValue* return_value = ThrowException) const {
         return C_Login(session,
                        user_type,
                        reinterpret_cast<Utf8Char*>(const_cast<uint8_t*>(pin.data())),
                        static_cast<Ulong>(pin.size()),
                        return_value);
      }

      /**
       * C_LoginUser logs a user into a token.
       *
       * @param session the session's handle
       * @param user_type the user type
       * @param pin_ptr the user's PIN
       * @param pin_len the length of the PIN
       * @param username_ptr the user's name
       * @param username_len the length of the user's name
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_LoginUser(SessionHandle session,
                       UserType user_type,
                       const Utf8Char* pin_ptr,
                       Ulong pin_len,
                       const Utf8Char* username_ptr,
                       Ulong username_len,
                       ReturnValue* return_value = ThrowException);

      /**
      * C_Logout logs a user out from a token.
      * @param session the session's handle
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li SessionClosed
      *     \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_Logout(SessionHandle session, ReturnValue* return_value = ThrowException) const;

      /**
       * C_GetSessionValidationFlags fetches the requested flags from the session. See
       * Validation indicators (section4.15.3.1) for meaning and semantics for these
       * flags. Applications are responsible for the appropriate locking to protect
       * session to get a meaningful result from this call.
       *
       * @param session the session's handle
       * @param type which state of flags
       * @param flags_ptr validation flags
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_GetSessionValidationFlags(SessionHandle session,
                                       Ulong type,
                                       Flags* flags_ptr,
                                       ReturnValue* return_value = ThrowException);

      /****************************** Object management functions ******************************/

      /**
      * C_CreateObject creates a new object.
      * @param session the session's handle
      * @param attribute_template_ptr the object's template
      * @param count attributes in template
      * @param object_ptr gets new object's handle.
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li AttributeReadOnly \li AttributeTypeInvalid
      *     \li AttributeValueInvalid \li CryptokiNotInitialized \li CurveNotSupported
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li DomainParamsInvalid \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li PinExpired
      *     \li SessionClosed \li SessionHandleInvalid \li SessionReadOnly
      *     \li TemplateIncomplete \li TemplateInconsistent \li TokenWriteProtected
      *     \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_CreateObject(SessionHandle session,
                          Attribute* attribute_template_ptr,
                          Ulong count,
                          ObjectHandle* object_ptr,
                          ReturnValue* return_value = ThrowException) const;

      /**
      * C_CopyObject copies an object, creating a new object for the copy.
      * @param session the session's handle
      * @param object the object's handle
      * @param attribute_template_ptr template for new object
      * @param count attributes in template
      * @param new_object_ptr receives handle of copy
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ActionProhibited \li ArgumentsBad \li AttributeReadOnly
      *     \li AttributeTypeInvalid \li AttributeValueInvalid \li CryptokiNotInitialized
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li ObjectHandleInvalid \li OK \li PinExpired
      *     \li SessionClosed \li SessionHandleInvalid \li SessionReadOnly
      *     \li TemplateInconsistent \li TokenWriteProtected \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_CopyObject(SessionHandle session,
                        ObjectHandle object,
                        Attribute* attribute_template_ptr,
                        Ulong count,
                        ObjectHandle* new_object_ptr,
                        ReturnValue* return_value = ThrowException) const;

      /**
      * C_DestroyObject destroys an object.
      * @param session the session's handle
      * @param object the object's handle
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ActionProhibited \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionFailed
      *     \li GeneralError \li HostMemory \li ObjectHandleInvalid
      *     \li OK \li PinExpired \li SessionClosed
      *     \li SessionHandleInvalid \li SessionReadOnly \li TokenWriteProtected
      * @return true on success, false otherwise
      */
      bool C_DestroyObject(SessionHandle session,
                           ObjectHandle object,
                           ReturnValue* return_value = ThrowException) const;

      /**
      * C_GetObjectSize gets the size of an object in bytes.
      * @param session the session's handle
      * @param object the object's handle
      * @param size_ptr receives size of object
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionFailed
      *     \li GeneralError \li HostMemory \li InformationSensitive
      *     \li ObjectHandleInvalid \li OK \li SessionClosed
      *     \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_GetObjectSize(SessionHandle session,
                           ObjectHandle object,
                           Ulong* size_ptr,
                           ReturnValue* return_value = ThrowException) const;

      /**
      * C_GetAttributeValue obtains the value of one or more object attributes.
      * @param session the session's handle
      * @param object the object's handle
      * @param attribute_template_ptr specifies attrs; gets vals
      * @param count attributes in template
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li AttributeSensitive \li AttributeTypeInvalid
      *     \li BufferTooSmall \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionFailed
      *     \li GeneralError \li HostMemory \li ObjectHandleInvalid
      *     \li OK \li SessionClosed \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_GetAttributeValue(SessionHandle session,
                               ObjectHandle object,
                               Attribute* attribute_template_ptr,
                               Ulong count,
                               ReturnValue* return_value = ThrowException) const;

      /**
      * C_GetAttributeValue obtains the value of one or more object attributes.
      * @param session the session's handle
      * @param object the object's handle
      * @param attribute_values specifies attrs; gets vals
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li AttributeSensitive \li AttributeTypeInvalid
      *     \li BufferTooSmall \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionFailed
      *     \li GeneralError \li HostMemory \li ObjectHandleInvalid
      *     \li OK \li SessionClosed \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      template <typename TAlloc>
      bool C_GetAttributeValue(SessionHandle session,
                               ObjectHandle object,
                               std::map<AttributeType, std::vector<uint8_t, TAlloc>>& attribute_values,
                               ReturnValue* return_value = ThrowException) const {
         std::vector<Attribute> getter_template;

         getter_template.reserve(attribute_values.size());
         for(const auto& entry : attribute_values) {
            getter_template.emplace_back(Attribute{static_cast<CK_ATTRIBUTE_TYPE>(entry.first), nullptr, 0});
         }

         const bool success = C_GetAttributeValue(session,
                                                  object,
                                                  const_cast<Attribute*>(getter_template.data()),
                                                  static_cast<Ulong>(getter_template.size()),
                                                  return_value);

         if(!success) {
            return success;
         }

         size_t i = 0;
         for(auto& entry : attribute_values) {
            entry.second.clear();
            entry.second.resize(getter_template.at(i).ulValueLen);
            getter_template.at(i).pValue = const_cast<uint8_t*>(entry.second.data());
            i++;
         }

         return C_GetAttributeValue(session,
                                    object,
                                    const_cast<Attribute*>(getter_template.data()),
                                    static_cast<Ulong>(getter_template.size()),
                                    return_value);
      }

      /**
      * C_SetAttributeValue modifies the value of one or more object attributes.
      * @param session the session's handle
      * @param object the object's handle
      * @param attribute_template_ptr specifies attrs and values
      * @param count attributes in template
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ActionProhibited \li ArgumentsBad \li AttributeReadOnly
      *     \li AttributeTypeInvalid \li AttributeValueInvalid \li CryptokiNotInitialized
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li ObjectHandleInvalid \li OK \li SessionClosed
      *     \li SessionHandleInvalid \li SessionReadOnly \li TemplateInconsistent
      *     \li TokenWriteProtected \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_SetAttributeValue(SessionHandle session,
                               ObjectHandle object,
                               Attribute* attribute_template_ptr,
                               Ulong count,
                               ReturnValue* return_value = ThrowException) const;

      /**
      * C_SetAttributeValue modifies the value of one or more object attributes.
      * @param session the session's handle
      * @param object the object's handle
      * @param attribute_values specifies attrs and values
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ActionProhibited \li ArgumentsBad \li AttributeReadOnly
      *     \li AttributeTypeInvalid \li AttributeValueInvalid \li CryptokiNotInitialized
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li ObjectHandleInvalid \li OK \li SessionClosed
      *     \li SessionHandleInvalid \li SessionReadOnly \li TemplateInconsistent
      *     \li TokenWriteProtected \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      template <typename TAlloc>
      bool C_SetAttributeValue(SessionHandle session,
                               ObjectHandle object,
                               std::map<AttributeType, std::vector<uint8_t, TAlloc>>& attribute_values,
                               ReturnValue* return_value = ThrowException) const {
         std::vector<Attribute> setter_template;

         setter_template.reserve(attribute_values.size());
         for(auto& entry : attribute_values) {
            setter_template.emplace_back(Attribute{static_cast<CK_ATTRIBUTE_TYPE>(entry.first),
                                                   entry.second.data(),
                                                   static_cast<CK_ULONG>(entry.second.size())});
         }

         return C_SetAttributeValue(session,
                                    object,
                                    const_cast<Attribute*>(setter_template.data()),
                                    static_cast<Ulong>(setter_template.size()),
                                    return_value);
      }

      /**
      * C_FindObjectsInit initializes a search for token and session objects that match a template.
      * @param session the session's handle
      * @param attribute_template_ptr attribute values to match
      * @param count attrs in search template
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li AttributeTypeInvalid \li AttributeValueInvalid
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li OperationActive
      *     \li PinExpired \li SessionClosed \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_FindObjectsInit(SessionHandle session,
                             Attribute* attribute_template_ptr,
                             Ulong count,
                             ReturnValue* return_value = ThrowException) const;

      /**
      * C_FindObjects continues a search for token and session objects that match a template, obtaining additional object handles.
      * @param session session's handle
      * @param object_ptr gets obj. handles
      * @param max_object_count max handles to get
      * @param object_count_ptr actual # returned
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li OperationNotInitialized \li SessionClosed \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_FindObjects(SessionHandle session,
                         ObjectHandle* object_ptr,
                         Ulong max_object_count,
                         Ulong* object_count_ptr,
                         ReturnValue* return_value = ThrowException) const;

      /**
      * C_FindObjectsFinal finishes a search for token and session objects.
      * @param session the session's handle
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li OperationNotInitialized
      *     \li SessionClosed \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_FindObjectsFinal(SessionHandle session, ReturnValue* return_value = ThrowException) const;

      /****************************** Encryption functions ******************************/

      /**
      * C_EncryptInit initializes an encryption operation.
      * @param session the session's handle
      * @param mechanism_ptr the encryption mechanism
      * @param key handle of encryption key
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li KeyFunctionNotPermitted
      *     \li KeyHandleInvalid \li KeySizeRange \li KeyTypeInconsistent
      *     \li MechanismInvalid \li MechanismParamInvalid \li OK
      *     \li OperationActive \li PinExpired \li SessionClosed
      *     \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_EncryptInit(SessionHandle session,
                         const Mechanism* mechanism_ptr,
                         ObjectHandle key,
                         ReturnValue* return_value = ThrowException) const;

      /**
      * C_Encrypt encrypts single-part data.
      * @param session session's handle
      * @param data_ptr the plaintext data
      * @param data_len size of plaintext data in bytes
      * @param encrypted_data gets ciphertext
      * @param encrypted_data_len_ptr gets c-text size
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DataInvalid \li DataLenRange \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationNotInitialized \li SessionClosed
      *     \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_Encrypt(SessionHandle session,
                     const Byte* data_ptr,
                     Ulong data_len,
                     Byte* encrypted_data,
                     Ulong* encrypted_data_len_ptr,
                     ReturnValue* return_value = ThrowException) const;

      /**
      * C_Encrypt encrypts single-part data.
      * @param session session's handle
      * @param plaintext_data the plaintext data
      * @param encrypted_data gets ciphertext
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DataInvalid \li DataLenRange \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationNotInitialized \li SessionClosed
      *     \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      template <typename TAllocA, typename TAllocB>
      bool C_Encrypt(SessionHandle session,
                     const std::vector<uint8_t, TAllocA>& plaintext_data,
                     std::vector<uint8_t, TAllocB>& encrypted_data,
                     ReturnValue* return_value = ThrowException) const {
         Ulong encrypted_size = 0;
         if(!C_Encrypt(session,
                       const_cast<Byte*>((plaintext_data.data())),
                       static_cast<Ulong>(plaintext_data.size()),
                       nullptr,
                       &encrypted_size,
                       return_value)) {
            return false;
         }

         encrypted_data.resize(encrypted_size);
         if(!C_Encrypt(session,
                       const_cast<Byte*>(plaintext_data.data()),
                       static_cast<Ulong>(plaintext_data.size()),
                       encrypted_data.data(),
                       &encrypted_size,
                       return_value)) {
            return false;
         }
         encrypted_data.resize(encrypted_size);
         return true;
      }

      /**
      * C_EncryptUpdate continues a multiple-part encryption operation.
      * @param session session's handle
      * @param part_ptr the plaintext data
      * @param part_len plaintext data len
      * @param encrypted_part_ptr gets ciphertext
      * @param encrypted_part_len_ptr gets c-text size
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DataLenRange \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li OperationNotInitialized \li SessionClosed \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_EncryptUpdate(SessionHandle session,
                           const Byte* part_ptr,
                           Ulong part_len,
                           Byte* encrypted_part_ptr,
                           Ulong* encrypted_part_len_ptr,
                           ReturnValue* return_value = ThrowException) const;

      /**
      * C_EncryptFinal finishes a multiple-part encryption operation.
      * @param session session handle
      * @param last_encrypted_part_ptr last c-text
      * @param last_encrypted_part_len_ptr gets last size
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DataLenRange \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li OperationNotInitialized \li SessionClosed \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_EncryptFinal(SessionHandle session,
                          Byte* last_encrypted_part_ptr,
                          Ulong* last_encrypted_part_len_ptr,
                          ReturnValue* return_value = ThrowException) const;

      /*********************** Message-based encryption functions ***********************/

      /**
       * C_MessageEncryptInit prepares a session for one or more encryption
       * operations that use the same encryption mechanism and
       * encryption key.
       *
       * @param session the session's handle
       * @param mechanism_ptr the encryption mechanism
       * @param key handle of encryption key
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_MessageEncryptInit(SessionHandle session,
                                const Mechanism* mechanism_ptr,
                                ObjectHandle key,
                                ReturnValue* return_value = ThrowException);

      /**
       * C_EncryptMessage encrypts a message in a single part.
       *
       * @param session the session's handle
       * @param parameter_ptr message specific parameter
       * @param parameter_len length of message specific parameter
       * @param associated_data_ptr AEAD Associated data
       * @param associated_data_len AEAD Associated data length
       * @param plaintext_ptr plain text
       * @param plaintext_len plain text length
       * @param ciphertext_ptr gets cipher text
       * @param ciphertext_len_ptr gets cipher text length
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_EncryptMessage(SessionHandle session,
                            const void* parameter_ptr,
                            Ulong parameter_len,
                            const Byte* associated_data_ptr,
                            Ulong associated_data_len,
                            const Byte* plaintext_ptr,
                            Ulong plaintext_len,
                            Byte* ciphertext_ptr,
                            Ulong* ciphertext_len_ptr,
                            ReturnValue* return_value = ThrowException);

      /**
       * C_EncryptMessageBegin begins a multiple-part message encryption operation.
       *
       * @param session the session's handle
       * @param parameter_ptr message specific parameter
       * @param parameter_len length of message specific parameter
       * @param associated_data_ptr AEAD Associated data
       * @param associated_data_len AEAD Associated data length
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_EncryptMessageBegin(SessionHandle session,
                                 const void* parameter_ptr,
                                 Ulong parameter_len,
                                 const Byte* associated_data_ptr,
                                 Ulong associated_data_len,
                                 ReturnValue* return_value = ThrowException);

      /**
       * C_EncryptMessageNext continues a multiple-part message encryption operation,
       * processing another message part.
       *
       * @param session the session's handle
       * @param parameter_ptr message specific parameter
       * @param parameter_len length of message specific parameter
       * @param plaintext_part_ptr plain text
       * @param plaintext_part_len plain text length
       * @param ciphertext_ptr gets cipher text
       * @param ciphertext_part_len_ptr gets cipher text length
       * @param flags multi mode flag
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_EncryptMessageNext(SessionHandle session,
                                const void* parameter_ptr,
                                Ulong parameter_len,
                                const Byte* plaintext_part_ptr,
                                Ulong plaintext_part_len,
                                Byte* ciphertext_ptr,
                                Ulong* ciphertext_part_len_ptr,
                                Flags flags,
                                ReturnValue* return_value = ThrowException);

      /**
       * C_MessageDecryptFinal finishes a message-based decryption process.
       *
       * @param session the session's handle
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_MessageEncryptFinal(SessionHandle session, ReturnValue* return_value = ThrowException);

      /****************************** Decryption functions ******************************/

      /**
      * C_DecryptInit initializes a decryption operation.
      * @param session the session's handle
      * @param mechanism_ptr the decryption mechanism
      * @param key handle of decryption key
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li KeyFunctionNotPermitted \li KeyHandleInvalid \li KeySizeRange
      *     \li KeyTypeInconsistent \li MechanismInvalid \li MechanismParamInvalid
      *     \li OK \li OperationActive \li PinExpired
      *     \li SessionClosed \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_DecryptInit(SessionHandle session,
                         const Mechanism* mechanism_ptr,
                         ObjectHandle key,
                         ReturnValue* return_value = ThrowException) const;

      /**
      * C_Decrypt decrypts encrypted data in a single part.
      * @param session session's handle
      * @param encrypted_data_ptr ciphertext
      * @param encrypted_data_len ciphertext length
      * @param data_ptr gets plaintext
      * @param data_len_ptr gets p-text size
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li EncryptedDataInvalid \li EncryptedDataLenRange \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationNotInitialized \li SessionClosed
      *     \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_Decrypt(SessionHandle session,
                     const Byte* encrypted_data_ptr,
                     Ulong encrypted_data_len,
                     Byte* data_ptr,
                     Ulong* data_len_ptr,
                     ReturnValue* return_value = ThrowException) const;

      /**
      * C_Decrypt decrypts encrypted data in a single part.
      * @param session session's handle
      * @param encrypted_data ciphertext
      * @param decrypted_data gets plaintext
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li EncryptedDataInvalid \li EncryptedDataLenRange \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationNotInitialized \li SessionClosed
      *     \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      template <typename TAllocA, typename TAllocB>
      bool C_Decrypt(SessionHandle session,
                     const std::vector<uint8_t, TAllocA>& encrypted_data,
                     std::vector<uint8_t, TAllocB>& decrypted_data,
                     ReturnValue* return_value = ThrowException) const {
         Ulong decrypted_size = 0;
         if(!C_Decrypt(session,
                       const_cast<Byte*>((encrypted_data.data())),
                       static_cast<Ulong>(encrypted_data.size()),
                       nullptr,
                       &decrypted_size,
                       return_value)) {
            return false;
         }

         decrypted_data.resize(decrypted_size);
         if(!C_Decrypt(session,
                       const_cast<Byte*>(encrypted_data.data()),
                       static_cast<Ulong>(encrypted_data.size()),
                       decrypted_data.data(),
                       &decrypted_size,
                       return_value)) {
            return false;
         }
         decrypted_data.resize(decrypted_size);
         return true;
      }

      /**
      * C_DecryptUpdate continues a multiple-part decryption operation.
      * @param session session's handle
      * @param encrypted_part_ptr encrypted data
      * @param encrypted_part_len input length
      * @param part_ptr gets plaintext
      * @param part_len_ptr p-text size
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li EncryptedDataInvalid \li EncryptedDataLenRange \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationNotInitialized \li SessionClosed
      *     \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_DecryptUpdate(SessionHandle session,
                           const Byte* encrypted_part_ptr,
                           Ulong encrypted_part_len,
                           Byte* part_ptr,
                           Ulong* part_len_ptr,
                           ReturnValue* return_value = ThrowException) const;

      /**
      * C_DecryptFinal finishes a multiple-part decryption operation.
      * @param session the session's handle
      * @param last_part_ptr gets plaintext
      * @param last_part_len_ptr p-text size
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li EncryptedDataInvalid \li EncryptedDataLenRange \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationNotInitialized \li SessionClosed
      *     \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_DecryptFinal(SessionHandle session,
                          Byte* last_part_ptr,
                          Ulong* last_part_len_ptr,
                          ReturnValue* return_value = ThrowException) const;

      /*********************** Message-based decryption functions ***********************/

      /**
       * C_MessageDecryptInit initializes a message-based decryption process,
       * preparing a session for one or more decryption operations that use the
       * same decryption mechanism and decryption key.
       *
       * @param session the session's handle
       * @param mechanism_ptr the decryption mechanism
       * @param key handle of decryption key
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_MessageDecryptInit(SessionHandle session,
                                const Mechanism* mechanism_ptr,
                                ObjectHandle key,
                                ReturnValue* return_value = ThrowException);

      /**
       * C_DecryptMessage decrypts an encrypted message in a single part.
       *
       * @param session the session's handle
       * @param parameter_ptr message specific parameter
       * @param parameter_len length of message specific parameter
       * @param associated_data_ptr AEAD Associated data
       * @param associated_data_len AEAD Associated data length
       * @param ciphertext_ptr cipher text
       * @param ciphertext_len cipher text length
       * @param plaintext_ptr gets plain text
       * @param plaintext_len_ptr gets plain text length
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_DecryptMessage(SessionHandle session,
                            const void* parameter_ptr,
                            Ulong parameter_len,
                            const Byte* associated_data_ptr,
                            Ulong associated_data_len,
                            const Byte* ciphertext_ptr,
                            Ulong ciphertext_len,
                            Byte* plaintext_ptr,
                            Ulong* plaintext_len_ptr,
                            ReturnValue* return_value = ThrowException);

      /**
       * C_DecryptMessageBegin begins a multiple-part message decryption operation.
       *
       * @param session the session's handle
       * @param parameter_ptr message specific parameter
       * @param parameter_len length of message specific parameter
       * @param associated_data_ptr AEAD Associated data
       * @param associated_data_len AEAD Associated data length
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_DecryptMessageBegin(SessionHandle session,
                                 const void* parameter_ptr,
                                 Ulong parameter_len,
                                 const Byte* associated_data_ptr,
                                 Ulong associated_data_len,
                                 ReturnValue* return_value = ThrowException);

      /**
       * C_DecryptMessageNext continues a multiple-part message decryption operation,
       * processing another encrypted message part.
       *
       * @param session the session's handle
       * @param parameter_ptr message specific parameter
       * @param parameter_len length of message specific parameter
       * @param ciphertext_part_ptr cipher text
       * @param ciphertext_part_len cipher text length
       * @param plaintext_ptr gets plain text
       * @param plaintext_part_len_ptr gets plain text length
       * @param flags multi mode flag
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_DecryptMessageNext(SessionHandle session,
                                const void* parameter_ptr,
                                Ulong parameter_len,
                                const Byte* ciphertext_part_ptr,
                                Ulong ciphertext_part_len,
                                Byte* plaintext_ptr,
                                Ulong* plaintext_part_len_ptr,
                                Flags flags,
                                ReturnValue* return_value = ThrowException);

      /**
       * C_MessageDecryptFinal finishes a message-based decryption process.
       *
       * @param session the session's handle
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_MessageDecryptFinal(SessionHandle session, ReturnValue* return_value = ThrowException);

      /****************************** Message digesting functions ******************************/

      /**
      * C_DigestInit initializes a message-digesting operation.
      * @param session the session's handle
      * @param mechanism_ptr the digesting mechanism
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li MechanismInvalid \li MechanismParamInvalid \li OK
      *     \li OperationActive \li PinExpired \li SessionClosed
      *     \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_DigestInit(SessionHandle session,
                        const Mechanism* mechanism_ptr,
                        ReturnValue* return_value = ThrowException) const;

      /**
      * C_Digest digests data in a single part.
      * @param session the session's handle
      * @param data_ptr data to be digested
      * @param data_len bytes of data to digest
      * @param digest_ptr gets the message digest
      * @param digest_len_ptr gets digest length
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li FunctionCanceled \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li OperationNotInitialized
      *     \li SessionClosed \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_Digest(SessionHandle session,
                    const Byte* data_ptr,
                    Ulong data_len,
                    Byte* digest_ptr,
                    Ulong* digest_len_ptr,
                    ReturnValue* return_value = ThrowException) const;

      /**
      * C_DigestUpdate continues a multiple-part message-digesting operation.
      * @param session the session's handle
      * @param part_ptr data to be digested
      * @param part_len bytes of data to be digested
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationNotInitialized \li SessionClosed
      *     \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_DigestUpdate(SessionHandle session,
                          const Byte* part_ptr,
                          Ulong part_len,
                          ReturnValue* return_value = ThrowException) const;

      /**
      * C_DigestKey continues a multi-part message-digesting operation, by digesting the value of a secret key as part of the data already digested.
      * @param session the session's handle
      * @param key secret key to digest
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li KeyHandleInvalid
      *     \li KeyIndigestible \li KeySizeRange \li OK
      *     \li OperationNotInitialized \li SessionClosed \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_DigestKey(SessionHandle session, ObjectHandle key, ReturnValue* return_value = ThrowException) const;

      /**
      * C_DigestFinal finishes a multiple-part message-digesting operation.
      * @param session the session's handle
      * @param digest_ptr gets the message digest
      * @param digest_len_ptr gets uint8_t count of digest
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li FunctionCanceled \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li OperationNotInitialized
      *     \li SessionClosed \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_DigestFinal(SessionHandle session,
                         Byte* digest_ptr,
                         Ulong* digest_len_ptr,
                         ReturnValue* return_value = ThrowException) const;

      /****************************** Signing and MACing functions ******************************/

      /**
      * C_SignInit initializes a signature (private key encryption) operation, where the signature is (will be) an appendix to the data, and plaintext cannot be recovered from the signature.
      * @param session the session's handle
      * @param mechanism_ptr the signature mechanism
      * @param key handle of signature key
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li KeyFunctionNotPermitted \li KeyHandleInvalid \li KeySizeRange
      *     \li KeyTypeInconsistent \li MechanismInvalid \li MechanismParamInvalid
      *     \li OK \li OperationActive \li PinExpired
      *     \li SessionClosed \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_SignInit(SessionHandle session,
                      const Mechanism* mechanism_ptr,
                      ObjectHandle key,
                      ReturnValue* return_value = ThrowException) const;

      /**
      * C_Sign signs (encrypts with private key) data in a single part, where the signature is (will be) an appendix to the data, and plaintext cannot be recovered from the signature.
      * @param session the session's handle
      * @param data_ptr the data to sign
      * @param data_len count of bytes to sign
      * @param signature_ptr gets the signature
      * @param signature_len_ptr gets signature length
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DataInvalid \li DataLenRange \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationNotInitialized \li SessionClosed
      *     \li SessionHandleInvalid \li UserNotLoggedIn \li FunctionRejected
      * @return true on success, false otherwise
      */
      bool C_Sign(SessionHandle session,
                  const Byte* data_ptr,
                  Ulong data_len,
                  Byte* signature_ptr,
                  Ulong* signature_len_ptr,
                  ReturnValue* return_value = ThrowException) const;

      /**
      * C_Sign signs (encrypts with private key) data in a single part, where the signature is (will be) an appendix to the data, and plaintext cannot be recovered from the signature.
      * @param session the session's handle
      * @param data the data to sign
      * @param signature gets the signature
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DataInvalid \li DataLenRange \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationNotInitialized \li SessionClosed
      *     \li SessionHandleInvalid \li UserNotLoggedIn \li FunctionRejected
      * @return true on success, false otherwise
      */
      template <typename TAllocA, typename TAllocB>
      bool C_Sign(SessionHandle session,
                  const std::vector<uint8_t, TAllocA>& data,
                  std::vector<uint8_t, TAllocB>& signature,
                  ReturnValue* return_value = ThrowException) const {
         Ulong signature_size = 0;
         if(!C_Sign(session, data.data(), static_cast<Ulong>(data.size()), nullptr, &signature_size, return_value)) {
            return false;
         }

         signature.resize(signature_size);
         if(!C_Sign(session,
                    data.data(),
                    static_cast<Ulong>(data.size()),
                    signature.data(),
                    &signature_size,
                    return_value)) {
            return false;
         }
         signature.resize(signature_size);
         return true;
      }

      /**
      * C_SignUpdate continues a multiple-part signature operation, where the signature is (will be) an appendix to the data, and plaintext cannot be recovered from the signature.
      * @param session the session's handle
      * @param part_ptr the data to sign
      * @param part_len count of bytes to sign
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DataLenRange
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li FunctionCanceled \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li OperationNotInitialized
      *     \li SessionClosed \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_SignUpdate(SessionHandle session,
                        const Byte* part_ptr,
                        Ulong part_len,
                        ReturnValue* return_value = ThrowException) const;

      /**
      * C_SignUpdate continues a multiple-part signature operation, where the signature is (will be) an appendix to the data, and plaintext cannot be recovered from the signature.
      * @param session the session's handle
      * @param part the data to sign
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DataLenRange
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li FunctionCanceled \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li OperationNotInitialized
      *     \li SessionClosed \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      template <typename TAlloc>
      bool C_SignUpdate(SessionHandle session,
                        const std::vector<uint8_t, TAlloc>& part,
                        ReturnValue* return_value = ThrowException) const {
         return C_SignUpdate(session, part.data(), static_cast<Ulong>(part.size()), return_value);
      }

      /**
      * C_SignFinal finishes a multiple-part signature operation, returning the signature.
      * @param session the session's handle
      * @param signature_ptr gets the signature
      * @param signature_len_ptr gets signature length
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DataLenRange \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li OperationNotInitialized \li SessionClosed \li SessionHandleInvalid
      *     \li UserNotLoggedIn \li FunctionRejected
      * @return true on success, false otherwise
      */
      bool C_SignFinal(SessionHandle session,
                       Byte* signature_ptr,
                       Ulong* signature_len_ptr,
                       ReturnValue* return_value = ThrowException) const;

      /**
      * C_SignFinal finishes a multiple-part signature operation, returning the signature.
      * @param session the session's handle
      * @param signature gets the signature
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DataLenRange \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li OperationNotInitialized \li SessionClosed \li SessionHandleInvalid
      *     \li UserNotLoggedIn \li FunctionRejected
      * @return true on success, false otherwise
      */
      template <typename TAlloc>
      bool C_SignFinal(SessionHandle session,
                       std::vector<uint8_t, TAlloc>& signature,
                       ReturnValue* return_value = ThrowException) const {
         Ulong signature_size = 0;
         if(!C_SignFinal(session, nullptr, &signature_size, return_value)) {
            return false;
         }

         signature.resize(signature_size);
         if(!C_SignFinal(session, signature.data(), &signature_size, return_value)) {
            return false;
         }
         signature.resize(signature_size);
         return true;
      }

      /**
      * C_SignRecoverInit initializes a signature operation, where the data can be recovered from the signature.
      * @param session the session's handle
      * @param mechanism_ptr the signature mechanism
      * @param key handle of the signature key
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li KeyFunctionNotPermitted \li KeyHandleInvalid \li KeySizeRange
      *     \li KeyTypeInconsistent \li MechanismInvalid \li MechanismParamInvalid
      *     \li OK \li OperationActive \li PinExpired
      *     \li SessionClosed \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_SignRecoverInit(SessionHandle session,
                             const Mechanism* mechanism_ptr,
                             ObjectHandle key,
                             ReturnValue* return_value = ThrowException) const;

      /**
      * C_SignRecover signs data in a single operation, where the data can be recovered from the signature.
      * @param session the session's handle
      * @param data_ptr the data to sign
      * @param data_len count of bytes to sign
      * @param signature_ptr gets the signature
      * @param signature_len_ptr gets signature length
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DataInvalid \li DataLenRange \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationNotInitialized \li SessionClosed
      *     \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_SignRecover(SessionHandle session,
                         const Byte* data_ptr,
                         Ulong data_len,
                         Byte* signature_ptr,
                         Ulong* signature_len_ptr,
                         ReturnValue* return_value = ThrowException) const;

      /******************* Message-based signing and MACing functions *******************/

      /**
       * C_MessageSignInit initializes a message-based signature process, preparing a
       * session for one or more signature operations (where the signature is an
       * appendix to the data) that use the same signature mechanism and
       * signature key.
       *
       * @param session the session's handle
       * @param mechanism_ptr the signing mechanism
       * @param key handle of signing key
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_MessageSignInit(SessionHandle session,
                             const Mechanism* mechanism_ptr,
                             ObjectHandle key,
                             ReturnValue* return_value = ThrowException);

      /**
       * C_SignMessage signs a message in a single part, where the signature is an
       * appendix to the message. C_MessageSignInit must previously been called
       * on the session.
       *
       * @param session the session's handle
       * @param parameter_ptr message specific parameter
       * @param parameter_len length of message specific parameter
       * @param data_ptr data to sign
       * @param data_len data to sign length
       * @param signature_ptr gets signature
       * @param signature_len_ptr gets signature length
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_SignMessage(SessionHandle session,
                         const void* parameter_ptr,
                         Ulong parameter_len,
                         const Byte* data_ptr,
                         Ulong data_len,
                         Byte* signature_ptr,
                         Ulong* signature_len_ptr,
                         ReturnValue* return_value = ThrowException);

      /**
       * C_SignMessageBegin begins a multiple-part message signature operation, where
       * the signature is an appendix to the message. C_MessageSignInit must
       * previously been called on the session.
       *
       * @param session the session's handle
       * @param parameter_ptr message specific parameter
       * @param parameter_len length of message specific parameter
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_SignMessageBegin(SessionHandle session,
                              const void* parameter_ptr,
                              Ulong parameter_len,
                              ReturnValue* return_value = ThrowException);

      /**
       * C_SignMessageNext continues a multiple-part message signature operation,
       * processing another data part, or finishes a multiple-part message
       * signature operation, returning the signature.
       *
       * @param session the session's handle
       * @param parameter_ptr message specific parameter
       * @param parameter_len length of message specific parameter
       * @param data_ptr data to sign
       * @param data_len data to sign length
       * @param signature_ptr gets signature
       * @param signature_len_ptr gets signature length
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_SignMessageNext(SessionHandle session,
                             const void* parameter_ptr,
                             Ulong parameter_len,
                             const Byte* data_ptr,
                             Ulong data_len,
                             Byte* signature_ptr,
                             Ulong* signature_len_ptr,
                             ReturnValue* return_value = ThrowException);

      /**
       * C_MessageSignFinal finishes a message-based signing process.
       *
       * @param session the session's handle
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_MessageSignFinal(SessionHandle session, ReturnValue* return_value = ThrowException);

      /****************************** Functions for verifying signatures and MACs ******************************/

      /**
      * C_VerifyInit initializes a verification operation, where the signature is an appendix to the data, and plaintext cannot be recovered from the signature (e.g. DSA).
      * @param session the session's handle
      * @param mechanism_ptr the verification mechanism
      * @param key verification key
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li KeyFunctionNotPermitted \li KeyHandleInvalid \li KeySizeRange
      *     \li KeyTypeInconsistent \li MechanismInvalid \li MechanismParamInvalid
      *     \li OK \li OperationActive \li PinExpired
      *     \li SessionClosed \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_VerifyInit(SessionHandle session,
                        const Mechanism* mechanism_ptr,
                        ObjectHandle key,
                        ReturnValue* return_value = ThrowException) const;

      /**
      * C_Verify verifies a signature in a single-part operation, where the signature is an appendix to the data, and plaintext cannot be recovered from the signature.
      * @param session the session's handle
      * @param data_ptr signed data
      * @param data_len length of signed data
      * @param signature_ptr signature
      * @param signature_len signature length
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DataInvalid
      *     \li DataLenRange \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li OperationNotInitialized \li SessionClosed \li SessionHandleInvalid
      *     \li SignatureInvalid \li SignatureLenRange
      * @return true on success, false otherwise
      */
      bool C_Verify(SessionHandle session,
                    const Byte* data_ptr,
                    Ulong data_len,
                    const Byte* signature_ptr,
                    Ulong signature_len,
                    ReturnValue* return_value = ThrowException) const;

      /**
      * C_Verify verifies a signature in a single-part operation, where the signature is an appendix to the data, and plaintext cannot be recovered from the signature.
      * @param session the session's handle
      * @param data signed data
      * @param signature signature
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DataInvalid
      *     \li DataLenRange \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li OperationNotInitialized \li SessionClosed \li SessionHandleInvalid
      *     \li SignatureInvalid \li SignatureLenRange
      * @return true on success, false otherwise
      */
      template <typename TAllocA, typename TAllocB>
      bool C_Verify(SessionHandle session,
                    const std::vector<uint8_t, TAllocA>& data,
                    std::vector<uint8_t, TAllocB>& signature,
                    ReturnValue* return_value = ThrowException) const {
         return C_Verify(session,
                         data.data(),
                         static_cast<Ulong>(data.size()),
                         signature.data(),
                         static_cast<Ulong>(signature.size()),
                         return_value);
      }

      /**
      * C_VerifyUpdate continues a multiple-part verification operation, where the signature is an appendix to the data, and plaintext cannot be recovered from the signature.
      * @param session the session's handle
      * @param part_ptr signed data
      * @param part_len length of signed data
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DataLenRange
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li FunctionCanceled \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li OperationNotInitialized
      *     \li SessionClosed \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_VerifyUpdate(SessionHandle session,
                          const Byte* part_ptr,
                          Ulong part_len,
                          ReturnValue* return_value = ThrowException) const;

      /**
      * C_VerifyUpdate continues a multiple-part verification operation, where the signature is an appendix to the data, and plaintext cannot be recovered from the signature.
      * @param session the session's handle
      * @param part signed data
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DataLenRange
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li FunctionCanceled \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li OperationNotInitialized
      *     \li SessionClosed \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      template <typename TAlloc>
      bool C_VerifyUpdate(SessionHandle session,
                          std::vector<uint8_t, TAlloc> part,
                          ReturnValue* return_value = ThrowException) const {
         return C_VerifyUpdate(session, part.data(), static_cast<Ulong>(part.size()), return_value);
      }

      /**
      * C_VerifyFinal finishes a multiple-part verification operation, checking the signature.
      * @param session the session's handle
      * @param signature_ptr signature to verify
      * @param signature_len signature length
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DataLenRange
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li FunctionCanceled \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li OperationNotInitialized
      *     \li SessionClosed \li SessionHandleInvalid \li SignatureInvalid
      *     \li SignatureLenRange
      * @return true on success, false otherwise
      */
      bool C_VerifyFinal(SessionHandle session,
                         const Byte* signature_ptr,
                         Ulong signature_len,
                         ReturnValue* return_value = ThrowException) const;

      /**
      * C_VerifyRecoverInit initializes a signature verification operation, where the data is recovered from the signature.
      * @param session the session's handle
      * @param mechanism_ptr the verification mechanism
      * @param key verification key
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li KeyFunctionNotPermitted \li KeyHandleInvalid \li KeySizeRange
      *     \li KeyTypeInconsistent \li MechanismInvalid \li MechanismParamInvalid
      *     \li OK \li OperationActive \li PinExpired
      *     \li SessionClosed \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_VerifyRecoverInit(SessionHandle session,
                               const Mechanism* mechanism_ptr,
                               ObjectHandle key,
                               ReturnValue* return_value = ThrowException) const;

      /**
      * C_VerifyRecover verifies a signature in a single-part operation, where the data is recovered from the signature.
      * @param session the session's handle
      * @param signature_ptr signature to verify
      * @param signature_len signature length
      * @param data_ptr gets signed data
      * @param data_len_ptr gets signed data len
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DataInvalid \li DataLenRange \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationNotInitialized \li SessionClosed
      *     \li SessionHandleInvalid \li SignatureLenRange \li SignatureInvalid
      * @return true on success, false otherwise
      */
      bool C_VerifyRecover(SessionHandle session,
                           const Byte* signature_ptr,
                           Ulong signature_len,
                           Byte* data_ptr,
                           Ulong* data_len_ptr,
                           ReturnValue* return_value = ThrowException) const;

      /**
       * C_VerifySignatureInit initializes a verification operation, where the
       * signature is included as part of the initialization.
       *
       * @param session the session's handle
       * @param mechanism_ptr the verification mechanism
       * @param key verification key
       * @param signature_ptr signature
       * @param signature_len signature length
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_VerifySignatureInit(SessionHandle session,
                                 const Mechanism* mechanism_ptr,
                                 ObjectHandle key,
                                 const Byte* signature_ptr,
                                 Ulong signature_len,
                                 ReturnValue* return_value = ThrowException);

      /**
       * C_VerifySignature verifies a signature in a single-part operation, where the
       * signature is an appendix to the data.
       *
       * @param session the session's handle
       * @param data_ptr signed data
       * @param data_len length of signed data
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_VerifySignature(SessionHandle session,
                             const Byte* data_ptr,
                             Ulong data_len,
                             ReturnValue* return_value = ThrowException);

      /**
       * C_VerifySignatureUpdate continues a multiple-part verification operation,
       * processing another data part.
       *
       * @param session the session's handle
       * @param part_ptr signed data
       * @param part_len length of signed data
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_VerifySignatureUpdate(SessionHandle session,
                                   const Byte* part_ptr,
                                   Ulong part_len,
                                   ReturnValue* return_value = ThrowException);

      /**
       * C_VerifySignatureFinal finishes a multiple-part verification operation,
       * checking the signature.
       *
       * @param session the session's handle
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_VerifySignatureFinal(SessionHandle session, ReturnValue* return_value = ThrowException);

      /*********** Message-based functions for verifying signatures and MACs ************/

      /**
       * C_MessageVerifyInit initializes a message-based verification process,
       * preparing a session for one or more verification operations (where the
       * signature is an appendix to the data) that use the same verification
       * mechanism and verification key.
       *
       * @param session the session's handle
       * @param mechanism_ptr the signing mechanism
       * @param key handle of signing key
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_MessageVerifyInit(SessionHandle session,
                               const Mechanism* mechanism_ptr,
                               ObjectHandle key,
                               ReturnValue* return_value = ThrowException);

      /**
       * C_VerifyMessage verifies a signature on a message in a single part operation,
       * where the signature is an appendix to the data. C_MessageVerifyInit must
       * previously been called on the session.
       *
       * @param session the session's handle
       * @param parameter_ptr message specific parameter
       * @param parameter_len length of message specific parameter
       * @param data_ptr data to sign
       * @param data_len data to sign length
       * @param signature_ptr signature
       * @param signature_len signature length
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_VerifyMessage(SessionHandle session,
                           const void* parameter_ptr,
                           Ulong parameter_len,
                           const Byte* data_ptr,
                           Ulong data_len,
                           const Byte* signature_ptr,
                           Ulong signature_len,
                           ReturnValue* return_value = ThrowException);

      /**
       * C_VerifyMessageBegin begins a multiple-part message verification operation,
       * where the signature is an appendix to the message. C_MessageVerifyInit
       * must previously been called on the session.
       *
       * @param session the session's handle
       * @param parameter_ptr message specific parameter
       * @param parameter_len length of message specific parameter
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_VerifyMessageBegin(SessionHandle session,
                                const void* parameter_ptr,
                                Ulong parameter_len,
                                ReturnValue* return_value = ThrowException);

      /**
       * C_VerifyMessageNext continues a multiple-part message verification operation,
       * processing another data part, or finishes a multiple-part message
       * verification operation, checking the signature.
       *
       * @param session the session's handle
       * @param parameter_ptr message specific parameter
       * @param parameter_len length of message specific parameter
       * @param data_ptr data to sign
       * @param data_len data to sign length
       * @param signature_ptr signature
       * @param signature_len signature length
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_VerifyMessageNext(SessionHandle session,
                               const void* parameter_ptr,
                               Ulong parameter_len,
                               const Byte* data_ptr,
                               Ulong data_len,
                               const Byte* signature_ptr,
                               Ulong signature_len,
                               ReturnValue* return_value = ThrowException);

      /**
       * C_MessageVerifyFinal finishes a message-based verification process.
       *
       * @param session the session's handle
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_MessageVerifyFinal(SessionHandle session, ReturnValue* return_value = ThrowException);

      /****************************** Dual-purpose cryptographic functions ******************************/

      /**
      * C_DigestEncryptUpdate continues a multiple-part digesting and encryption operation.
      * @param session session's handle
      * @param part_ptr the plaintext data
      * @param part_len plaintext length
      * @param encrypted_part_ptr gets ciphertext
      * @param encrypted_part_len_ptr gets c-text length
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DataLenRange \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li OperationNotInitialized \li SessionClosed \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_DigestEncryptUpdate(SessionHandle session,
                                 const Byte* part_ptr,
                                 Ulong part_len,
                                 Byte* encrypted_part_ptr,
                                 Ulong* encrypted_part_len_ptr,
                                 ReturnValue* return_value = ThrowException) const;

      /**
      * C_DecryptDigestUpdate continues a multiple-part decryption and digesting operation.
      * @param session session's handle
      * @param encrypted_part_ptr ciphertext
      * @param encrypted_part_len ciphertext length
      * @param part_ptr gets plaintext
      * @param part_len_ptr gets plaintext len
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li EncryptedDataInvalid \li EncryptedDataLenRange \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationNotInitialized \li SessionClosed
      *     \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_DecryptDigestUpdate(SessionHandle session,
                                 const Byte* encrypted_part_ptr,
                                 Ulong encrypted_part_len,
                                 Byte* part_ptr,
                                 Ulong* part_len_ptr,
                                 ReturnValue* return_value = ThrowException) const;

      /**
      * C_SignEncryptUpdate continues a multiple-part signing and encryption operation.
      * @param session session's handle
      * @param part_ptr the plaintext data
      * @param part_len plaintext length
      * @param encrypted_part_ptr gets ciphertext
      * @param encrypted_part_len_ptr gets c-text length
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DataLenRange \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li OK
      *     \li OperationNotInitialized \li SessionClosed \li SessionHandleInvalid
      *     \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_SignEncryptUpdate(SessionHandle session,
                               const Byte* part_ptr,
                               Ulong part_len,
                               Byte* encrypted_part_ptr,
                               Ulong* encrypted_part_len_ptr,
                               ReturnValue* return_value = ThrowException) const;

      /**
      * C_DecryptVerifyUpdate continues a multiple-part decryption and verify operation.
      * @param session session's handle
      * @param encrypted_part_ptr ciphertext
      * @param encrypted_part_len ciphertext length
      * @param part_ptr gets plaintext
      * @param part_len_ptr gets p-text length
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DataLenRange \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li EncryptedDataInvalid \li EncryptedDataLenRange
      *     \li FunctionCanceled \li FunctionFailed \li GeneralError
      *     \li HostMemory \li OK \li OperationNotInitialized
      *     \li SessionClosed \li SessionHandleInvalid
      * @return true on success, false otherwise
      */
      bool C_DecryptVerifyUpdate(SessionHandle session,
                                 const Byte* encrypted_part_ptr,
                                 Ulong encrypted_part_len,
                                 Byte* part_ptr,
                                 Ulong* part_len_ptr,
                                 ReturnValue* return_value = ThrowException) const;

      /****************************** Key management functions ******************************/

      /**
      * C_GenerateKey generates a secret key, creating a new key object.
      * @param session the session's handle
      * @param mechanism_ptr key generation mech.
      * @param attribute_template_ptr template for new key
      * @param count # of attrs in template
      * @param key_ptr gets handle of new key
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li AttributeReadOnly \li AttributeTypeInvalid
      *     \li AttributeValueInvalid \li CryptokiNotInitialized \li CurveNotSupported
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li FunctionCanceled \li FunctionFailed \li GeneralError
      *     \li HostMemory \li MechanismInvalid \li MechanismParamInvalid
      *     \li OK \li OperationActive \li PinExpired
      *     \li SessionClosed \li SessionHandleInvalid \li SessionReadOnly
      *     \li TemplateIncomplete \li TemplateInconsistent \li TokenWriteProtected
      *     \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_GenerateKey(SessionHandle session,
                         const Mechanism* mechanism_ptr,
                         Attribute* attribute_template_ptr,
                         Ulong count,
                         ObjectHandle* key_ptr,
                         ReturnValue* return_value = ThrowException) const;

      /**
      * C_GenerateKeyPair generates a public-key/private-key pair, creating new key objects.
      * @param session session handle
      * @param mechanism_ptr key-gen mech.
      * @param public_key_template_ptr template for pub. key
      * @param public_key_attribute_count # pub. attrs.
      * @param private_key_template_ptr template for priv. key
      * @param private_key_attribute_count # priv.  attrs.
      * @param public_key_ptr gets pub. key handle
      * @param private_key_ptr gets priv. key handle
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li AttributeReadOnly \li AttributeTypeInvalid
      *     \li AttributeValueInvalid \li CryptokiNotInitialized \li CurveNotSupported
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li DomainParamsInvalid \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li MechanismInvalid
      *     \li MechanismParamInvalid \li OK \li OperationActive
      *     \li PinExpired \li SessionClosed \li SessionHandleInvalid
      *     \li SessionReadOnly \li TemplateIncomplete \li TemplateInconsistent
      *     \li TokenWriteProtected \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_GenerateKeyPair(SessionHandle session,
                             const Mechanism* mechanism_ptr,
                             Attribute* public_key_template_ptr,
                             Ulong public_key_attribute_count,
                             Attribute* private_key_template_ptr,
                             Ulong private_key_attribute_count,
                             ObjectHandle* public_key_ptr,
                             ObjectHandle* private_key_ptr,
                             ReturnValue* return_value = ThrowException) const;

      /**
      * C_WrapKey wraps (i.e., encrypts) a key.
      * @param session the session's handle
      * @param mechanism_ptr the wrapping mechanism
      * @param wrapping_key wrapping key
      * @param key key to be wrapped
      * @param wrapped_key_ptr gets wrapped key
      * @param wrapped_key_len_ptr gets wrapped key size
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li BufferTooSmall \li CryptokiNotInitialized
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li FunctionCanceled \li FunctionFailed \li GeneralError
      *     \li HostMemory \li KeyHandleInvalid \li KeyNotWrappable
      *     \li KeySizeRange \li KeyUnextractable \li MechanismInvalid
      *     \li MechanismParamInvalid \li OK \li OperationActive
      *     \li PinExpired \li SessionClosed \li SessionHandleInvalid
      *     \li UserNotLoggedIn \li WrappingKeyHandleInvalid \li WrappingKeySizeRange
      *     \li WrappingKeyTypeInconsistent
      * @return true on success, false otherwise
      */
      bool C_WrapKey(SessionHandle session,
                     const Mechanism* mechanism_ptr,
                     ObjectHandle wrapping_key,
                     ObjectHandle key,
                     Byte* wrapped_key_ptr,
                     Ulong* wrapped_key_len_ptr,
                     ReturnValue* return_value = ThrowException) const;

      /**
      * C_UnwrapKey unwraps (decrypts) a wrapped key, creating a new key object.
      * @param session session's handle
      * @param mechanism_ptr unwrapping mech.
      * @param unwrapping_key unwrapping key
      * @param wrapped_key_ptr the wrapped key
      * @param wrapped_key_len wrapped key len
      * @param attribute_template_ptr new key template
      * @param attribute_count template length
      * @param key_ptr gets new handle
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li AttributeReadOnly \li AttributeTypeInvalid
      *     \li AttributeValueInvalid \li BufferTooSmall \li CryptokiNotInitialized
      *     \li CurveNotSupported \li DeviceError \li DeviceMemory
      *     \li DeviceRemoved \li DomainParamsInvalid \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li MechanismInvalid \li MechanismParamInvalid \li OK
      *     \li OperationActive \li PinExpired \li SessionClosed
      *     \li SessionHandleInvalid \li SessionReadOnly \li TemplateIncomplete
      *     \li TemplateInconsistent \li TokenWriteProtected \li UnwrappingKeyHandleInvalid
      *     \li UnwrappingKeySizeRange \li UnwrappingKeyTypeInconsistent \li UserNotLoggedIn
      *     \li WrappedKeyInvalid \li WrappedKeyLenRange
      * @return true on success, false otherwise
      */
      bool C_UnwrapKey(SessionHandle session,
                       const Mechanism* mechanism_ptr,
                       ObjectHandle unwrapping_key,
                       const Byte* wrapped_key_ptr,
                       Ulong wrapped_key_len,
                       Attribute* attribute_template_ptr,
                       Ulong attribute_count,
                       ObjectHandle* key_ptr,
                       ReturnValue* return_value = ThrowException) const;

      /**
      * C_DeriveKey derives a key from a base key, creating a new key object.
      * @param session session's handle
      * @param mechanism_ptr key deriv. mech.
      * @param base_key base key
      * @param attribute_template_ptr new key template
      * @param attribute_count template length
      * @param key_ptr gets new handle
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li AttributeReadOnly \li AttributeTypeInvalid
      *     \li AttributeValueInvalid \li CryptokiNotInitialized \li CurveNotSupported
      *     \li DeviceError \li DeviceMemory \li DeviceRemoved
      *     \li DomainParamsInvalid \li FunctionCanceled \li FunctionFailed
      *     \li GeneralError \li HostMemory \li KeyHandleInvalid
      *     \li KeySizeRange \li KeyTypeInconsistent \li MechanismInvalid
      *     \li MechanismParamInvalid \li OK \li OperationActive
      *     \li PinExpired \li SessionClosed \li SessionHandleInvalid
      *     \li SessionReadOnly \li TemplateIncomplete \li TemplateInconsistent
      *     \li TokenWriteProtected \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_DeriveKey(SessionHandle session,
                       const Mechanism* mechanism_ptr,
                       ObjectHandle base_key,
                       Attribute* attribute_template_ptr,
                       Ulong attribute_count,
                       ObjectHandle* key_ptr,
                       ReturnValue* return_value = ThrowException) const;

      /**
       * C_WrapKeyAuthenticated wraps (i.e. encrypts) a private or secret key.
       *
       * @param session session's handle
       * @param mechanism_ptr wrapping mechanism
       * @param wrapping_key wrapping key
       * @param key key to be wrapped
       * @param associated_data_ptr associated data for an AEAD mechanism
       * @param associated_data_len length of the associated data
       * @param wrapped_key_ptr gets the wrapped key
       * @param wrapped_key_len_ptr gets the length of the wrapped key
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_WrapKeyAuthenticated(SessionHandle session,
                                  const Mechanism* mechanism_ptr,
                                  ObjectHandle wrapping_key,
                                  ObjectHandle key,
                                  const Byte* associated_data_ptr,
                                  Ulong associated_data_len,
                                  Byte* wrapped_key_ptr,
                                  Ulong* wrapped_key_len_ptr,
                                  ReturnValue* return_value = ThrowException) const;

      /**
       * C_UnwrapKeyAuthenticated unwraps (i.e. decrypts) a wrapped key,
       * creating a new private key or secret key object.
       *
       * @param session session's handle
       * @param mechanism_ptr unwrapping mechanism
       * @param unwrapping_key unwrapping key
       * @param wrapped_key_ptr wrapped key
       * @param wrapped_key_len length of the wrapped key
       * @param attribute_template_ptr new key template
       * @param attribute_count template length
       * @param associated_data_ptr associated data for an AEAD mechanism
       * @param associated_data_len length of the associated data
       * @param key_ptr gets new key handle
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_UnwrapKeyAuthenticated(SessionHandle session,
                                    const Mechanism* mechanism_ptr,
                                    ObjectHandle unwrapping_key,
                                    const Byte* wrapped_key_ptr,
                                    Ulong wrapped_key_len,
                                    Attribute* attribute_template_ptr,
                                    Ulong attribute_count,
                                    const Byte* associated_data_ptr,
                                    Ulong associated_data_len,
                                    ObjectHandle* key_ptr,
                                    ReturnValue* return_value = ThrowException) const;

      /**
       * C_EncapulateKey creates a new secret key object from a public key using a
       * KEM.
       *
       * @param session the session's handle
       * @param mechanism_ptr the encapsulation mechanism
       * @param public_key the encapsulating key
       * @param template_ptr new key template
       * @param attribute_count template length
       * @param ciphertext_ptr the wrapped key
       * @param ciphertext_len_ptr the wrapped key size
       * @param key_ptr the encapsulated key
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_EncapsulateKey(SessionHandle session,
                            const Mechanism* mechanism_ptr,
                            ObjectHandle public_key,
                            Attribute* template_ptr,
                            Ulong attribute_count,
                            Byte* ciphertext_ptr,
                            Ulong* ciphertext_len_ptr,
                            ObjectHandle* key_ptr,
                            ReturnValue* return_value = ThrowException);

      /**
       * C_DecapsulateKey creates a new secret key object based on the private key and
       * ciphertext generated by a prior encapsulate operation. This new key
       * (called a ‘shared key’ in most KEM documentation) is identical to the
       * key returned by C_EncapsulateKey when it was called with the matching public
       * key and returned the same cipher text. This function is a KEM style
       * function.
       *
       * @param session the session's handle
       * @param mechanism_ptr the decapsulation mechanism
       * @param private_key the decapsulating key
       * @param template_ptr new key template
       * @param attribute_count template length
       * @param ciphertext_ptr the wrapped key
       * @param ciphertext_len the wrapped key size
       * @param key_ptr the decapsulated key
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_DecapsulateKey(SessionHandle session,
                            const Mechanism* mechanism_ptr,
                            ObjectHandle private_key,
                            Attribute* template_ptr,
                            Ulong attribute_count,
                            const Byte* ciphertext_ptr,
                            Ulong ciphertext_len,
                            ObjectHandle* key_ptr,
                            ReturnValue* return_value = ThrowException);

      /****************************** Random number generation functions ******************************/

      /**
      * C_SeedRandom mixes additional seed material into the token's random number generator.
      * @param session the session's handle
      * @param seed_ptr the seed material
      * @param seed_len length of seed material
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationActive \li RandomSeedNotSupported
      *     \li RandomNoRng \li SessionClosed \li SessionHandleInvalid
      *     \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_SeedRandom(SessionHandle session,
                        const Byte* seed_ptr,
                        Ulong seed_len,
                        ReturnValue* return_value = ThrowException) const;

      /**
      * C_GenerateRandom generates random data.
      * @param session the session's handle
      * @param random_data_ptr receives the random data
      * @param random_len # of bytes to generate
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li ArgumentsBad \li CryptokiNotInitialized \li DeviceError
      *     \li DeviceMemory \li DeviceRemoved \li FunctionCanceled
      *     \li FunctionFailed \li GeneralError \li HostMemory
      *     \li OK \li OperationActive \li RandomNoRng
      *     \li SessionClosed \li SessionHandleInvalid \li UserNotLoggedIn
      * @return true on success, false otherwise
      */
      bool C_GenerateRandom(SessionHandle session,
                            Byte* random_data_ptr,
                            Ulong random_len,
                            ReturnValue* return_value = ThrowException) const;

      /****************************** Parallel function management functions ******************************/

      /**
      * C_GetFunctionStatus is a legacy function; it obtains an updated status of a function running in parallel with an application.
      * @param session the session's handle
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li FunctionFailed \li FunctionNotParallel
      *     \li GeneralError \li HostMemory \li SessionHandleInvalid
      *     \li SessionClosed
      * @return true on success, false otherwise
      */
      bool C_GetFunctionStatus(SessionHandle session, ReturnValue* return_value = ThrowException) const;

      /**
      * C_CancelFunction is a legacy function; it cancels a function running in parallel.
      * @param session the session's handle
      * @param return_value default value (`ThrowException`): throw exception on error.
      * if a non-NULL pointer is passed: return_value receives the return value of the PKCS #11 function and no exception is thrown.
      * At least the following PKCS #11 return values may be returned:
      *     \li CryptokiNotInitialized \li FunctionFailed \li FunctionNotParallel
      *     \li GeneralError \li HostMemory \li SessionHandleInvalid
      *     \li SessionClosed
      * @return true on success, false otherwise
      */
      bool C_CancelFunction(SessionHandle session, ReturnValue* return_value = ThrowException) const;

      /******************* Asynchronous function management functions *******************/

      /**
       * C_AsyncComplete checks if the function identified by function_name_ptr has
       * completed an asynchronous operation and, if so, returns the associated
       * result(s).
       *
       * @param session the session's handle
       * @param function_name_ptr pkcs11 function name
       * @param result_ptr operation result
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_AsyncComplete(SessionHandle session,
                           const Utf8Char* function_name_ptr,
                           AsyncData* result_ptr,
                           ReturnValue* return_value = ThrowException);

      /**
       * C_AsyncGetID is used to persist an operation past a C_Finalize call and allow
       * another instance of the client to reconnect after a call to
       * C_Initialize. C_AsyncGetID places a module dependent identifier for
       * the asynchronous operation being performed by the function identified by
       * function_name_ptr.
       *
       * @param session the session's handle
       * @param function_name_ptr pkcs11 function name
       * @param id_ptr persistent operation id
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_AsyncGetID(SessionHandle session,
                        const Utf8Char* function_name_ptr,
                        Ulong* id_ptr,
                        ReturnValue* return_value = ThrowException);

      /**
       * C_AsyncJoin checks if the function identified by function_name_ptr and id is a
       * valid asynchronous operation and, if so, reconnects the client application to
       * the module using the buffer specified by data_ptr and data_len in place of those
       * passed into the original call to function_name_ptr.
       *
       * @param session the session's handle
       * @param function_name_ptr pkcs11 function name
       * @param id persistent operation id
       * @param data_ptr location for the data
       * @param data_len data length
       * @param return_value default value (`ThrowException`): throw exception on error
       * @return true on success, false otherwise
       */
      bool C_AsyncJoin(SessionHandle session,
                       const Utf8Char* function_name_ptr,
                       Ulong id,
                       Byte* data_ptr,
                       Ulong data_len,
                       ReturnValue* return_value = ThrowException);

      /**
      * Return the PKCS11 function list that this LowLevel class contains.
      *
      * This is primarily useful when invoking vendor specific extension
      * functions which are not supported directly by LowLevel or the higher
      * level PKCS11 API.
      */
      BOTAN_DEPRECATED("Use get_interface().func_2_40()") FunctionList* get_functions() const;

      const InterfaceWrapper& get_interface() { return m_interface_wrapper; }

   protected:
      /**
      * it is possible for an application to inherit from LowLevel in order to
      * implement wrappers for vendor specific extensions using the same error
      * handling mechanisms as the rest of the library.
      */
      static bool handle_return_value(CK_RV function_result, ReturnValue* return_value);

   private:
      InterfaceWrapper m_interface_wrapper;
};

class BOTAN_PUBLIC_API(2, 0) PKCS11_Error : public Exception {
   public:
      explicit PKCS11_Error(std::string_view what) : Exception("PKCS11 error", what) {}

      ErrorType error_type() const noexcept override { return ErrorType::Pkcs11Error; }
};

class BOTAN_PUBLIC_API(2, 0) PKCS11_ReturnError final : public PKCS11_Error {
   public:
      explicit PKCS11_ReturnError(ReturnValue return_val);

      inline ReturnValue get_return_value() const { return m_return_val; }

      int error_code() const noexcept override { return static_cast<int>(m_return_val); }

   private:
      const ReturnValue m_return_val;
};

}  // namespace PKCS11

}  // namespace Botan

namespace Botan {

class Dynamically_Loaded_Library;

namespace PKCS11 {

/**
* Loads the PKCS#11 shared library
* Calls C_Initialize on load and C_Finalize on destruction
*/
class BOTAN_PUBLIC_API(2, 0) Module final {
   public:
      /**
      * Loads the shared library and calls C_Initialize. The latest supported
      * "PKCS 11" interface is used.
      * @param file_path the path to the PKCS#11 shared library
      * @param init_args flags to use for `C_Initialize`
      */
      BOTAN_FUTURE_EXPLICIT Module(
         std::string_view file_path,
         C_InitializeArgs init_args = {
            nullptr, nullptr, nullptr, nullptr, static_cast<CK_FLAGS>(Flag::OsLockingOk), nullptr});

      Module(Module&& other) noexcept;
      Module& operator=(Module&& other) = delete;

      // Dtor calls C_Finalize(). A copy could be deleted while the origin still exists
      // Furthermore std::unique_ptr member -> not copyable
      Module(const Module& other) = delete;
      Module& operator=(const Module& other) = delete;

      /// Calls C_Finalize()
      ~Module() noexcept;

      /**
      * Reloads the module and reinitializes it.
      * @param init_args flags to use for `C_Initialize`
      */
      void reload(C_InitializeArgs init_args = {
                     nullptr, nullptr, nullptr, nullptr, static_cast<CK_FLAGS>(Flag::OsLockingOk), nullptr});

      inline LowLevel* operator->() const { return m_low_level.get(); }

      /// @return general information about Cryptoki
      inline Info get_info() const {
         Info info;
         m_low_level->C_GetInfo(&info);
         return info;
      }

      std::string_view library_path() const { return m_file_path; }

      const Dynamically_Loaded_Library& library() { return *m_library; }

   private:
      const std::string m_file_path;
      std::unique_ptr<Dynamically_Loaded_Library> m_library;
      std::unique_ptr<LowLevel> m_low_level = nullptr;
};

/// Represents a PKCS#11 Slot, i.e., a card reader
class BOTAN_PUBLIC_API(2, 0) Slot final {
   public:
      /**
      * @param module the PKCS#11 module to use
      * @param slot_id the slot id to use
      */
      Slot(Module& module, SlotId slot_id);

      /// @return a reference to the module that is used
      inline Module& module() const { return m_module; }

      /// @return the slot id
      inline SlotId slot_id() const { return m_slot_id; }

      /**
      * Get available slots
      * @param module the module to use
      * @param token_present true if only slots with attached tokens should be returned, false for all slots
      * @return a list of available slots (calls C_GetSlotList)
      */
      static std::vector<SlotId> get_available_slots(Module& module, bool token_present);

      /// @return information about the slot (`C_GetSlotInfo`)
      SlotInfo get_slot_info() const;

      /// Obtains a list of mechanism types supported by the slot (`C_GetMechanismList`)
      std::vector<MechanismType> get_mechanism_list() const;

      /// Obtains information about a particular mechanism possibly supported by a slot (`C_GetMechanismInfo`)
      MechanismInfo get_mechanism_info(MechanismType mechanism_type) const;

      /// Obtains information about a particular token in the system (`C_GetTokenInfo`)
      TokenInfo get_token_info() const;

      /**
      * Calls `C_InitToken` to initialize the token
      * @param label the label for the token (must not exceed 32 bytes according to PKCS#11)
      * @param so_pin the PIN of the security officer
      */
      void initialize(std::string_view label, const secure_string& so_pin) const;

   private:
      const std::reference_wrapper<Module> m_module;
      const SlotId m_slot_id;
};

/// Represents a PKCS#11 session
class BOTAN_PUBLIC_API(2, 0) Session final {
   public:
      /**
      * @param slot the slot to use
      * @param read_only true if the session should be read only, false to create a read-write session
      */
      Session(Slot& slot, bool read_only);

      /**
      * @param slot the slot to use
      * @param flags the flags to use for the session. Remark: Flag::SerialSession is mandatory
      * @param callback_data application-defined pointer to be passed to the notification callback
      * @param notify_callback address of the notification callback function
      */
      Session(Slot& slot, Flags flags, VoidPtr callback_data, Notify notify_callback);

      /// Takes ownership of a session
      Session(Slot& slot, SessionHandle handle);

      Session(Session&& other) = default;
      Session& operator=(Session&& other) = delete;

      // Dtor calls C_CloseSession() and eventually C_Logout. A copy could close the session while the origin still exists
      Session(const Session& other) = delete;
      Session& operator=(const Session& other) = delete;

      /// Logout user and close the session on destruction
      ~Session() noexcept;

      /// @return a reference to the slot
      inline const Slot& slot() const { return m_slot; }

      /// @return the session handle of this session
      inline SessionHandle handle() const { return m_handle; }

      /// @return a reference to the used module
      inline Module& module() const { return m_slot.module(); }

      /// @return the released session handle
      SessionHandle release();

      /**
      * Login to this session
      * @param userType the user type to use for the login
      * @param pin the PIN of the user
      */
      void login(UserType userType, const secure_string& pin);

      /// Logout from this session
      void logoff();

      /// @return information about this session
      SessionInfo get_info() const;

      /// Calls `C_SetPIN` to change the PIN using the old PIN (requires a logged in session)
      void set_pin(const secure_string& old_pin, const secure_string& new_pin);

      /// Calls `C_InitPIN` to change or initialize the PIN using the SO_PIN (requires a logged in session)
      void init_pin(const secure_string& new_pin);

   private:
      const Slot& m_slot;
      SessionHandle m_handle;
      bool m_logged_in;
};

}  // namespace PKCS11
}  // namespace Botan


namespace Botan::PKCS11 {

class Module;

/// Helper class to build the Attribute / CK_ATTRIBUTE structures
class BOTAN_PUBLIC_API(2, 0) AttributeContainer {
   public:
      AttributeContainer() = default;

      /// @param object_class the class type of this container
      BOTAN_FUTURE_EXPLICIT AttributeContainer(ObjectClass object_class);

      virtual ~AttributeContainer() = default;

      AttributeContainer(AttributeContainer&& other) = default;
      AttributeContainer& operator=(AttributeContainer&& other) = default;

      // Warning when implementing copy/assignment: m_attributes contains pointers to the other members which must be updated after a copy
      AttributeContainer(const AttributeContainer& other) = delete;
      AttributeContainer& operator=(const AttributeContainer& other) = delete;

      /// @return the attributes this container contains
      inline const std::vector<Attribute>& attributes() const { return m_attributes; }

      /// @return raw attribute data
      inline Attribute* data() const { return const_cast<Attribute*>(m_attributes.data()); }

      /// @return the number of attributes in this container
      inline size_t count() const { return m_attributes.size(); }

      /**
      * Add a class attribute (CKA_CLASS / AttributeType::Class).
      * @param object_class class attribute to add
      */
      void add_class(ObjectClass object_class);

      /**
      * Add a string attribute (e.g. CKA_LABEL / AttributeType::Label).
      * @param attribute attribute type
      * @param value string value to add
      */
      void add_string(AttributeType attribute, std::string_view value);

      /**
      * Add a binary attribute (e.g. CKA_ID / AttributeType::Id).
      * @param attribute attribute type
      * @param value binary attribute value to add
      * @param length size of the binary attribute value in bytes
      */
      void add_binary(AttributeType attribute, const uint8_t* value, size_t length);

      /**
      * Add a binary attribute (e.g. CKA_ID / AttributeType::Id).
      * @param attribute attribute type
      * @param binary binary attribute value to add
      */
      template <typename TAlloc>
      void add_binary(AttributeType attribute, const std::vector<uint8_t, TAlloc>& binary) {
         add_binary(attribute, binary.data(), binary.size());
      }

      /**
      * Add a bool attribute (e.g. CKA_SENSITIVE / AttributeType::Sensitive).
      * @param attribute attribute type
      * @param value boolean value to add
      */
      void add_bool(AttributeType attribute, bool value);

      /**
      * Add a numeric attribute (e.g. CKA_MODULUS_BITS / AttributeType::ModulusBits).
      * @param attribute attribute type
      * @param value numeric value to add
      */
      template <std::integral T>
      void add_numeric(AttributeType attribute, T value) {
         m_numerics.push_back(static_cast<uint64_t>(value));
         add_attribute(attribute, reinterpret_cast<uint8_t*>(&m_numerics.back()), sizeof(T));
      }

   protected:
      /// Add an attribute with the given value and size to the attribute collection `m_attributes`
      void add_attribute(AttributeType attribute, const uint8_t* value, Ulong size);

   private:
      std::vector<Attribute> m_attributes;
      std::list<uint64_t> m_numerics;
      std::list<std::string> m_strings;
      std::list<secure_vector<uint8_t>> m_vectors;
};

/// Manages calls to C_FindObjects* functions (C_FindObjectsInit -> C_FindObjects -> C_FindObjectsFinal)
class BOTAN_PUBLIC_API(2, 0) ObjectFinder final {
   public:
      /**
      * Initializes a search for token and session objects that match a template (calls C_FindObjectsInit)
      * @param session the session to use for the search
      * @param search_template the search_template as a vector of `Attribute`
      */
      ObjectFinder(Session& session, const std::vector<Attribute>& search_template);

      ObjectFinder(const ObjectFinder& other) = default;
      ObjectFinder& operator=(const ObjectFinder& other) = delete;

      ObjectFinder(ObjectFinder&& other) = default;
      ObjectFinder& operator=(ObjectFinder&& other) = delete;

      /// Terminates a search for token and session objects (calls C_FindObjectsFinal)
      ~ObjectFinder() noexcept;

      /**
      * Starts or continues a search for token and session objects that match a template, obtaining additional object handles (calls C_FindObjects)
      * @param max_count maximum amount of object handles to retrieve. Default = 100
      * @return the result of the search as a vector of `ObjectHandle`
      */
      std::vector<ObjectHandle> find(std::uint32_t max_count = 100) const;

      /// Finishes the search operation manually to allow a new ObjectFinder to exist
      void finish();

      /// @return the module this `ObjectFinder` belongs to
      inline Module& module() const { return m_session.get().module(); }

   private:
      const std::reference_wrapper<Session> m_session;
      bool m_search_terminated;
};

/// Common attributes of all objects
class BOTAN_PUBLIC_API(2, 0) ObjectProperties : public AttributeContainer {
   public:
      /// @param object_class the object class of the object
      BOTAN_FUTURE_EXPLICIT ObjectProperties(ObjectClass object_class);

      /// @return the object class of this object
      inline ObjectClass object_class() const { return m_object_class; }

   private:
      const ObjectClass m_object_class;
};

/// Common attributes of all storage objects
class BOTAN_PUBLIC_API(2, 0) StorageObjectProperties : public ObjectProperties {
   public:
      /// @param object_class the CK_OBJECT_CLASS this storage object belongs to
      BOTAN_FUTURE_EXPLICIT StorageObjectProperties(ObjectClass object_class);

      /// @param label description of the object (RFC2279 string)
      inline void set_label(std::string_view label) { add_string(AttributeType::Label, label); }

      /// @param value if true the object is a token object; otherwise the object is a session object
      inline void set_token(bool value) { add_bool(AttributeType::Token, value); }

      /**
      * @param value if true the object is a private object; otherwise the object is a public object
      * When private, a user may not access the object until the user has been authenticated to the token
      */
      inline void set_private(bool value) { add_bool(AttributeType::Private, value); }

      /// @param value if true the object can be modified, otherwise it is read-only
      void set_modifiable(bool value) { add_bool(AttributeType::Modifiable, value); }

      /// @param value if true the object can be copied using C_CopyObject
      void set_copyable(bool value) { add_bool(AttributeType::Copyable, value); }

      /// @param value if true the object can be destroyed using C_DestroyObject
      void set_destroyable(bool value) { add_bool(AttributeType::Destroyable, value); }
};

/// Common attributes of all data objects
class BOTAN_PUBLIC_API(2, 0) DataObjectProperties final : public StorageObjectProperties {
   public:
      DataObjectProperties();

      /// @param value description of the application that manages the object (RFC2279 string)
      inline void set_application(std::string_view value) { add_string(AttributeType::Application, value); }

      /// @param object_id DER-encoding of the object identifier indicating the data object type
      inline void set_object_id(const std::vector<uint8_t>& object_id) {
         add_binary(AttributeType::ObjectId, object_id);
      }

      /// @param value value of the object
      inline void set_value(const secure_vector<uint8_t>& value) { add_binary(AttributeType::Value, value); }
};

/// Common attributes of all certificate objects
class BOTAN_PUBLIC_API(2, 0) CertificateProperties : public StorageObjectProperties {
   public:
      /// @param cert_type type of certificate
      BOTAN_FUTURE_EXPLICIT CertificateProperties(CertificateType cert_type);

      /// @param value the certificate can be trusted for the application that it was created (can only be set to true by SO user)
      inline void set_trusted(bool value) { add_bool(AttributeType::Trusted, value); }

      /// @param category one of `CertificateCategory`
      inline void set_category(CertificateCategory category) {
         add_numeric(AttributeType::CertificateCategory, static_cast<CK_CERTIFICATE_CATEGORY>(category));
      }

      /**
      * @param checksum the value of this attribute is derived from the certificate by taking the
      * first three bytes of the SHA - 1 hash of the certificate object's `CKA_VALUE` attribute
      */
      inline void set_check_value(const std::vector<uint8_t>& checksum) {
         add_binary(AttributeType::CheckValue, checksum);
      }

      /// @param date start date for the certificate
      inline void set_start_date(Date date) {
         add_binary(AttributeType::StartDate, reinterpret_cast<uint8_t*>(&date), sizeof(Date));
      }

      /// @param date end date for the certificate
      inline void set_end_date(Date date) {
         add_binary(AttributeType::EndDate, reinterpret_cast<uint8_t*>(&date), sizeof(Date));
      }

      /// @param pubkey_info DER-encoding of the SubjectPublicKeyInfo for the public key contained in this certificate
      inline void set_public_key_info(const std::vector<uint8_t>& pubkey_info) {
         add_binary(AttributeType::PublicKeyInfo, pubkey_info);
      }

      /// @return the certificate type of this certificate object
      inline CertificateType cert_type() const { return m_cert_type; }

   private:
      const CertificateType m_cert_type;
};

/// Common attributes of all key objects
class BOTAN_PUBLIC_API(2, 0) KeyProperties : public StorageObjectProperties {
   public:
      /**
      * @param object_class the `CK_OBJECT_CLASS` this key object belongs to
      * @param key_type type of key
      */
      KeyProperties(ObjectClass object_class, KeyType key_type);

      /// @param id key identifier for key
      inline void set_id(const std::vector<uint8_t>& id) { add_binary(AttributeType::Id, id); }

      /// @param date start date for the key
      inline void set_start_date(Date date) {
         add_binary(AttributeType::StartDate, reinterpret_cast<uint8_t*>(&date), sizeof(Date));
      }

      /// @param date end date for the key
      inline void set_end_date(Date date) {
         add_binary(AttributeType::EndDate, reinterpret_cast<uint8_t*>(&date), sizeof(Date));
      }

      /// @param value true if key supports key derivation (i.e., if other keys can be derived from this one)
      inline void set_derive(bool value) { add_bool(AttributeType::Derive, value); }

      /**
      * Sets a list of mechanisms allowed to be used with this key
      * Not implemented
      * TODO(Botan4) remove this
      */
      inline void set_allowed_mechanisms(
         const std::vector<MechanismType>& /*mechanisms*/) {  // NOLINT(*-convert-member-functions-to-static)
         throw Not_Implemented("KeyProperties::set_allowed_mechanisms");
      }

      /// @return the key type of this key object
      inline KeyType key_type() const { return m_key_type; }

   private:
      const KeyType m_key_type;
};

/// Common attributes of all public key objects
class BOTAN_PUBLIC_API(2, 0) PublicKeyProperties : public KeyProperties {
   public:
      /// @param key_type type of key
      BOTAN_FUTURE_EXPLICIT PublicKeyProperties(KeyType key_type);

      /// @param subject DER-encoding of the key subject name
      inline void set_subject(const std::vector<uint8_t>& subject) { add_binary(AttributeType::Subject, subject); }

      /// @param value true if the key supports encryption
      inline void set_encrypt(bool value) { add_bool(AttributeType::Encrypt, value); }

      /// @param value true if the key supports verification where the signature is an appendix to the data
      inline void set_verify(bool value) { add_bool(AttributeType::Verify, value); }

      /// @param value true if the key supports verification where the data is recovered from the signature
      inline void set_verify_recover(bool value) { add_bool(AttributeType::VerifyRecover, value); }

      /// @param value true if the key supports wrapping (i.e., can be used to wrap other keys)
      inline void set_wrap(bool value) { add_bool(AttributeType::Wrap, value); }

      /**
      * @param value true if the key can be trusted for the application that it was created.
      * The wrapping key can be used to wrap keys with `CKA_WRAP_WITH_TRUSTED` set to `CK_TRUE`
      */
      inline void set_trusted(bool value) { add_bool(AttributeType::Trusted, value); }

      /**
      * For wrapping keys
      * The attribute template to match against any keys wrapped using this wrapping key.
      * Keys that do not match cannot be wrapped
      * Not implemented
      * TODO(Botan4) remove this function
      */
      inline void set_wrap_template(
         const AttributeContainer& /*unused*/) {  // NOLINT(*-convert-member-functions-to-static)
         throw Not_Implemented("PublicKeyProperties::set_wrap_template");
      }

      /// @param pubkey_info DER-encoding of the SubjectPublicKeyInfo for this public key
      inline void set_public_key_info(const std::vector<uint8_t>& pubkey_info) {
         add_binary(AttributeType::PublicKeyInfo, pubkey_info);
      }
};

/// Common attributes of all private keys
class BOTAN_PUBLIC_API(2, 0) PrivateKeyProperties : public KeyProperties {
   public:
      /// @param key_type type of key
      BOTAN_FUTURE_EXPLICIT PrivateKeyProperties(KeyType key_type);

      /// @param subject DER-encoding of the key subject name
      inline void set_subject(const std::vector<uint8_t>& subject) { add_binary(AttributeType::Subject, subject); }

      /// @param value true if the key is sensitive
      inline void set_sensitive(bool value) { add_bool(AttributeType::Sensitive, value); }

      /// @param value true if the key supports decryption
      inline void set_decrypt(bool value) { add_bool(AttributeType::Decrypt, value); }

      /// @param value true if the key supports signatures where the signature is an appendix to the data
      inline void set_sign(bool value) { add_bool(AttributeType::Sign, value); }

      /// @param value true if the key supports signatures where the data can be recovered from the signature
      inline void set_sign_recover(bool value) { add_bool(AttributeType::SignRecover, value); }

      /// @param value true if the key supports unwrapping (i.e., can be used to unwrap other keys)
      inline void set_unwrap(bool value) { add_bool(AttributeType::Unwrap, value); }

      /// @param value true if the key is extractable and can be wrapped
      inline void set_extractable(bool value) { add_bool(AttributeType::Extractable, value); }

      /// @param value true if the key can only be wrapped with a wrapping key that has `CKA_TRUSTED` set to `CK_TRUE`
      inline void set_wrap_with_trusted(bool value) { add_bool(AttributeType::WrapWithTrusted, value); }

      /// @param value If true, the user has to supply the PIN for each use (sign or decrypt) with the key
      inline void set_always_authenticate(bool value) { add_bool(AttributeType::AlwaysAuthenticate, value); }

      /**
      * For wrapping keys
      * The attribute template to apply to any keys unwrapped using this wrapping key.
      * Any user supplied template is applied after this template as if the object has already been created
      * Not implemented
      * TODO(Botan4) remove this function
      */
      inline void set_unwrap_template(
         const AttributeContainer& /*unused*/) {  // NOLINT(*-convert-member-functions-to-static)
         throw Not_Implemented("PrivateKeyProperties::set_unwrap_template");
      }

      /// @param pubkey_info DER-encoding of the SubjectPublicKeyInfo for this public key
      inline void set_public_key_info(const std::vector<uint8_t>& pubkey_info) {
         add_binary(AttributeType::PublicKeyInfo, pubkey_info);
      }
};

/// Common attributes of all secret (symmetric) keys
class BOTAN_PUBLIC_API(2, 0) SecretKeyProperties final : public KeyProperties {
   public:
      /// @param key_type type of key
      BOTAN_FUTURE_EXPLICIT SecretKeyProperties(KeyType key_type);

      /// @param value true if the key is sensitive
      inline void set_sensitive(bool value) { add_bool(AttributeType::Sensitive, value); }

      /// @param value true if the key supports encryption
      inline void set_encrypt(bool value) { add_bool(AttributeType::Encrypt, value); }

      /// @param value true if the key supports decryption
      inline void set_decrypt(bool value) { add_bool(AttributeType::Decrypt, value); }

      /// @param value true if the key supports signatures where the signature is an appendix to the data
      inline void set_sign(bool value) { add_bool(AttributeType::Sign, value); }

      /// @param value true if the key supports verification where the signature is an appendix to the data
      inline void set_verify(bool value) { add_bool(AttributeType::Verify, value); }

      /// @param value true if the key supports unwrapping (i.e., can be used to unwrap other keys)
      inline void set_unwrap(bool value) { add_bool(AttributeType::Unwrap, value); }

      /// @param value true if the key is extractable and can be wrapped
      inline void set_extractable(bool value) { add_bool(AttributeType::Extractable, value); }

      /// @param value true if the key can only be wrapped with a wrapping key that has `CKA_TRUSTED` set to `CK_TRUE`
      inline void set_wrap_with_trusted(bool value) { add_bool(AttributeType::WrapWithTrusted, value); }

      /// @param value if true, the user has to supply the PIN for each use (sign or decrypt) with the key
      inline void set_always_authenticate(bool value) { add_bool(AttributeType::AlwaysAuthenticate, value); }

      /// @param value true if the key supports wrapping (i.e., can be used to wrap other keys)
      inline void set_wrap(bool value) { add_bool(AttributeType::Wrap, value); }

      /**
      * @param value the key can be trusted for the application that it was created.
      * The wrapping key can be used to wrap keys with `CKA_WRAP_WITH_TRUSTED` set to `CK_TRUE`
      */
      inline void set_trusted(bool value) { add_bool(AttributeType::Trusted, value); }

      /// @param checksum the key check value of this key
      inline void set_check_value(const std::vector<uint8_t>& checksum) {
         add_binary(AttributeType::CheckValue, checksum);
      }

      /**
      * For wrapping keys
      * The attribute template to match against any keys wrapped using this wrapping key.
      * Keys that do not match cannot be wrapped
      * Not implemented
      * TODO(Botan4) remove this function
      */
      inline void set_wrap_template(
         const AttributeContainer& /*unused*/) {  // NOLINT(*-convert-member-functions-to-static)
         throw Not_Implemented("SecretKeyProperties::set_wrap_template");
      }

      /**
      * For wrapping keys
      * The attribute template to apply to any keys unwrapped using this wrapping key
      * Any user supplied template is applied after this template as if the object has already been created
      * Not Implemented
      * TODO(Botan4) remove this function
      */
      inline void set_unwrap_template(
         const AttributeContainer& /*unused*/) {  // NOLINT(*-convert-member-functions-to-static)
         throw Not_Implemented("SecretKeyProperties::set_unwrap_template");
      }
};

/// Common attributes of domain parameter
class BOTAN_PUBLIC_API(2, 0) DomainParameterProperties final : public StorageObjectProperties {
   public:
      /// @param key_type type of key the domain parameters can be used to generate
      BOTAN_FUTURE_EXPLICIT DomainParameterProperties(KeyType key_type);

      /// @return the key type
      inline KeyType key_type() const { return m_key_type; }

   private:
      const KeyType m_key_type;
};

/**
* Represents a PKCS#11 object.
*/
class BOTAN_PUBLIC_API(2, 0) Object {
   public:
      /**
      * Creates an `Object` from an existing PKCS#11 object
      * @param session the session the object belongs to
      * @param handle handle of the object
      */

      Object(Session& session, ObjectHandle handle);

      /**
      * Creates the object
      * @param session the session in which the object should be created
      * @param obj_props properties of this object
      */
      Object(Session& session, const ObjectProperties& obj_props);

      Object(const Object&) = default;
      Object(Object&&) = default;
      Object& operator=(const Object&) = delete;
      Object& operator=(Object&&) = delete;

      virtual ~Object() = default;

      /// Searches for all objects of the given type that match `search_template`
      template <typename T>
      static std::vector<T> search(Session& session, const std::vector<Attribute>& search_template);

      /// Searches for all objects of the given type using the label (`CKA_LABEL`)
      template <typename T>
      static std::vector<T> search(Session& session, std::string_view label);

      /// Searches for all objects of the given type using the id (`CKA_ID`)
      template <typename T>
      static std::vector<T> search(Session& session, const std::vector<uint8_t>& id);

      /// Searches for all objects of the given type using the label (`CKA_LABEL`) and id (`CKA_ID`)
      template <typename T>
      static std::vector<T> search(Session& session, std::string_view label, const std::vector<uint8_t>& id);

      /// Searches for all objects of the given type
      template <typename T>
      static std::vector<T> search(Session& session);

      /// @returns the value of the given attribute (using `C_GetAttributeValue`)
      secure_vector<uint8_t> get_attribute_value(AttributeType attribute) const;

      /// Sets the given value for the attribute (using `C_SetAttributeValue`)
      void set_attribute_value(AttributeType attribute, const secure_vector<uint8_t>& value) const;

      /// Destroys the object
      void destroy() const;

      /**
      * Copies the object
      * @param modified_attributes the attributes of the copied object
      */
      ObjectHandle copy(const AttributeContainer& modified_attributes) const;

      /// @return the handle of this object.
      inline ObjectHandle handle() const { return m_handle; }

      /// @return the session this objects belongs to
      inline Session& session() const { return m_session; }

      /// @return the module this object belongs to
      inline Module& module() const { return m_session.get().module(); }

   protected:
      explicit Object(Session& session) : m_session(session) {}

      void reset_handle(ObjectHandle handle) {
         if(m_handle != CK_INVALID_HANDLE) {
            throw Invalid_Argument("Cannot reset handle on already valid PKCS11 object");
         }
         m_handle = handle;
      }

   private:
      const std::reference_wrapper<Session> m_session;
      ObjectHandle m_handle = CK_INVALID_HANDLE;
};

template <typename T>
std::vector<T> Object::search(Session& session, const std::vector<Attribute>& search_template) {
   const ObjectFinder finder(session, search_template);
   const std::vector<ObjectHandle> handles = finder.find();
   std::vector<T> result;
   result.reserve(handles.size());
   for(const auto& handle : handles) {
      result.emplace_back(T(session, handle));
   }
   return result;
}

template <typename T>
std::vector<T> Object::search(Session& session, std::string_view label) {
   AttributeContainer search_template(T::Class);
   search_template.add_string(AttributeType::Label, label);
   return search<T>(session, search_template.attributes());
}

template <typename T>
std::vector<T> Object::search(Session& session, const std::vector<uint8_t>& id) {
   AttributeContainer search_template(T::Class);
   search_template.add_binary(AttributeType::Id, id);
   return search<T>(session, search_template.attributes());
}

template <typename T>
std::vector<T> Object::search(Session& session, std::string_view label, const std::vector<uint8_t>& id) {
   AttributeContainer search_template(T::Class);
   search_template.add_string(AttributeType::Label, label);
   search_template.add_binary(AttributeType::Id, id);
   return search<T>(session, search_template.attributes());
}

template <typename T>
std::vector<T> Object::search(Session& session) {
   return search<T>(session, AttributeContainer(T::Class).attributes());
}

}  // namespace Botan::PKCS11

#if defined(BOTAN_HAS_ECC_PUBLIC_KEY_CRYPTO)
   #include <vector>

namespace Botan::PKCS11 {

class Session;

/// Properties for generating a PKCS#11 EC public key
class BOTAN_PUBLIC_API(2, 0) EC_PublicKeyGenerationProperties final : public PublicKeyProperties {
   public:
      /// @param ec_params DER-encoding of an ANSI X9.62 Parameters value
      BOTAN_FUTURE_EXPLICIT EC_PublicKeyGenerationProperties(const std::vector<uint8_t>& ec_params);

      /// @return the DER-encoding of the ec parameters according to ANSI X9.62
      inline const std::vector<uint8_t>& ec_params() const { return m_ec_params; }

   private:
      const std::vector<uint8_t> m_ec_params;
};

/// Properties for importing a PKCS#11 EC public key
class BOTAN_PUBLIC_API(2, 0) EC_PublicKeyImportProperties final : public PublicKeyProperties {
   public:
      /**
      * @param ec_params DER-encoding of an ANSI X9.62 Parameters value
      * @param ec_point DER-encoding of ANSI X9.62 ECPoint value Q
      */
      EC_PublicKeyImportProperties(const std::vector<uint8_t>& ec_params, const std::vector<uint8_t>& ec_point);

      /// @return the DER-encoding of the ec parameters according to ANSI X9.62
      inline const std::vector<uint8_t>& ec_params() const { return m_ec_params; }

      /// @return the DER-encoding of the ec public point according to ANSI X9.62
      inline const std::vector<uint8_t>& ec_point() const { return m_ec_point; }

   private:
      const std::vector<uint8_t> m_ec_params;
      const std::vector<uint8_t> m_ec_point;
};

/// Represents a PKCS#11 EC public key
class BOTAN_PUBLIC_API(2, 0) PKCS11_EC_PublicKey : public virtual EC_PublicKey,
                                                   public Object {
   public:
      static const ObjectClass Class = ObjectClass::PublicKey;

      /**
      * Creates a PKCS11_EC_PublicKey object from an existing PKCS#11 EC public key
      * @param session the session to use
      * @param handle the handle of the ecc public key
      */
      PKCS11_EC_PublicKey(Session& session, ObjectHandle handle);

      /**
      * Imports an EC public key
      * @param session the session to use
      * @param props the attributes of the public key
      */
      PKCS11_EC_PublicKey(Session& session, const EC_PublicKeyImportProperties& props);
};

/// Properties for generating a PKCS#11 EC private key
class BOTAN_PUBLIC_API(2, 0) EC_PrivateKeyGenerationProperties final : public PrivateKeyProperties {
   public:
      EC_PrivateKeyGenerationProperties() : PrivateKeyProperties(KeyType::Ec) {}
};

/// Properties for importing a PKCS#11 EC private key
class BOTAN_PUBLIC_API(2, 0) EC_PrivateKeyImportProperties final : public PrivateKeyProperties {
   public:
      /**
      * @param ec_params DER-encoding of an ANSI X9.62 Parameters value
      * @param value ANSI X9.62 private value d
      */
      EC_PrivateKeyImportProperties(const std::vector<uint8_t>& ec_params, const BigInt& value);

      /// @return the DER-encoding of the ec parameters according to ANSI X9.62
      inline const std::vector<uint8_t>& ec_params() const { return m_ec_params; }

      /// @return the value of the ec private key
      inline const BigInt& value() const { return m_value; }

   private:
      const std::vector<uint8_t> m_ec_params;
      const BigInt m_value;
};

// note: don't inherit from PKCS11_EC_PublicKey: a private key object IS NOT A public key object on a smartcard (-> two different objects)
// note: don't inherit from EC_PublicKey: the public key can not be extracted from a PKCS11-EC-PrivateKey (its only attributes are CKA_EC_PARAMS and CKA_VALUE)
/// Represents a PKCS#11 EC private key
class BOTAN_PUBLIC_API(2, 0) PKCS11_EC_PrivateKey : public virtual Private_Key,
                                                    public Object {
   public:
      static const ObjectClass Class = ObjectClass::PrivateKey;

      /**
      * Creates a PKCS11_EC_PrivateKey object from an existing PKCS#11 EC private key
      * @param session the session to use
      * @param handle the handle of the EC private key
      */
      PKCS11_EC_PrivateKey(Session& session, ObjectHandle handle);

      /**
      * Imports an EC private key
      * @param session the session to use
      * @param props the attributes of the private key
      */
      PKCS11_EC_PrivateKey(Session& session, const EC_PrivateKeyImportProperties& props);

      /**
      * Generates a PKCS#11 EC private key
      * @param session the session to use
      * @param ec_params DER-encoding of an ANSI X9.62 Parameters value
      * @param props the attributes of the private key
      * @note no persistent public key object will be created
      */
      PKCS11_EC_PrivateKey(Session& session,
                           const std::vector<uint8_t>& ec_params,
                           const EC_PrivateKeyGenerationProperties& props);

      /// @returns the domain of the EC private key
      inline const EC_Group& domain() const { return m_domain_params; }

   #if defined(BOTAN_HAS_LEGACY_EC_POINT)
      /**
      * Sets the associated public point of this private key
      * @param point the public point
      * @param point_encoding encoding of the point (default DER-encoded)
      */
      void set_public_point(const EC_Point& point, PublicPointEncoding point_encoding = PublicPointEncoding::Der) {
         this->set_public_point(EC_AffinePoint(domain(), point), point_encoding);
      }
   #endif

      /**
      * Sets the associated public point of this private key
      * @param point the public point
      * @param point_encoding encoding of the point (default DER-encoded)
      */
      void set_public_point(const EC_AffinePoint& point,
                            PublicPointEncoding point_encoding = PublicPointEncoding::Der) {
         m_public_key = point;
         m_point_encoding = point_encoding;
      }

      /**
       * Sets the public desired public point encoding of this private key, when it is passed to cryptoki functions.
       * This could be either `PublicPointEncoding::Raw` or `PublicPointEncoding::Der`. By default this is set to `Der`,
       * but some tokens might expect `Raw`-encoded public keys, e.g. when using this private key for key agreement.
       */
      void set_point_encoding(PublicPointEncoding point_encoding) { m_point_encoding = point_encoding; }

   #if defined(BOTAN_HAS_LEGACY_EC_POINT)
      /**
      * Gets the public_point
      * @note the public key must be set using `set_public_point`
      *       because it is not possible to infer the public key from a PKCS#11 EC private key
      * @return the public point of the private key
      * @throws Exception if the public point was not set using set_public_point()
      */
      EC_Point public_point() const { return this->public_ec_point().to_legacy_point(); }
   #endif

      /**
      * Gets the elliptic curve point associated with the public key
      *
      * @note the public key must be set using `set_public_point` because it is
      *       not possible to infer the public key from a PKCS#11 EC private key
      *
      * @return the public point of the private key
      * @throws Exception if the public point was not set using set_public_point()
      */
      EC_AffinePoint public_ec_point() const {
         if(m_public_key) {
            return m_public_key.value();
         } else {
            throw Invalid_State(
               "Public point not set. Inferring the public key from a PKCS#11 ec private key is not possible.");
         }
      }

      /// @return the encoding format for the public point when it is passed to cryptoki functions as an argument
      PublicPointEncoding point_encoding() const { return m_point_encoding; }

      // Private_Key methods

      std::vector<uint8_t> raw_public_key_bits() const override;

      std::vector<uint8_t> public_key_bits() const override;

      std::size_t key_length() const override;

      std::size_t estimated_strength() const override;

      bool check_key(RandomNumberGenerator& rng, bool strong) const override;

      AlgorithmIdentifier algorithm_identifier() const override;

   private:
      EC_Group m_domain_params;
      std::optional<EC_AffinePoint> m_public_key;
      PublicPointEncoding m_point_encoding = PublicPointEncoding::Der;
};
}  // namespace Botan::PKCS11

#endif

#if defined(BOTAN_HAS_ECDH)


   #include <string>
   #include <vector>

namespace Botan::PKCS11 {
class Session;

/// Represents a PKCS#11 ECDH public key
class BOTAN_PUBLIC_API(2, 0) PKCS11_ECDH_PublicKey : public PKCS11_EC_PublicKey {
   public:
      /**
      * Create a PKCS11_ECDH_PublicKey object from an existing PKCS#11 ECDH public key
      * @param session the session to use
      * @param handle the handle of the ECDH public key
      */
      PKCS11_ECDH_PublicKey(Session& session, ObjectHandle handle) : PKCS11_EC_PublicKey(session, handle) {}

      /**
      * Imports a ECDH public key
      * @param session the session to use
      * @param props the attributes of the public key
      */
      PKCS11_ECDH_PublicKey(Session& session, const EC_PublicKeyImportProperties& props) :
            PKCS11_EC_PublicKey(session, props) {}

      inline std::string algo_name() const override { return "ECDH"; }

      /**
       * @throws Not_Implemented as this operation is not possible in PKCS11
       */
      std::unique_ptr<Private_Key> generate_another(RandomNumberGenerator& /*rng*/) const final {
         throw Not_Implemented("Cannot generate a new PKCS#11 ECDH keypair from this public key");
      }

      bool supports_operation(PublicKeyOperation op) const override { return (op == PublicKeyOperation::KeyAgreement); }

      /// @return the exported ECDH public key
      ECDH_PublicKey export_key() const;
};

/// Represents a PKCS#11 ECDH private key

BOTAN_DIAGNOSTIC_PUSH
BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE

class BOTAN_PUBLIC_API(2, 0) PKCS11_ECDH_PrivateKey final : public virtual PKCS11_EC_PrivateKey,
                                                            public virtual PK_Key_Agreement_Key {
   public:
      /**
      * Creates a PKCS11_ECDH_PrivateKey object from an existing PKCS#11 ECDH private key
      * @param session the session to use
      * @param handle the handle of the ECDH private key
      */
      PKCS11_ECDH_PrivateKey(Session& session, ObjectHandle handle) : PKCS11_EC_PrivateKey(session, handle) {}

      /**
      * Imports an ECDH private key
      * @param session the session to use
      * @param props the attributes of the private key
      */
      PKCS11_ECDH_PrivateKey(Session& session, const EC_PrivateKeyImportProperties& props) :
            PKCS11_EC_PrivateKey(session, props) {}

      /**
      * Generates a PKCS#11 ECDH private key
      * @param session the session to use
      * @param ec_params DER-encoding of an ANSI X9.62 Parameters value
      * @param props the attributes of the private key
      * @note no persistent public key object will be created
      */
      PKCS11_ECDH_PrivateKey(Session& session,
                             const std::vector<uint8_t>& ec_params,
                             const EC_PrivateKeyGenerationProperties& props) :
            PKCS11_EC_PrivateKey(session, ec_params, props) {}

      inline std::string algo_name() const override { return "ECDH"; }

      std::unique_ptr<Public_Key> public_key() const override;

      inline std::vector<uint8_t> public_value() const override { return public_ec_point().serialize_uncompressed(); }

      /// @return the exported ECDH private key
      ECDH_PrivateKey export_key() const;

      secure_vector<uint8_t> private_key_bits() const override;

      /**
       * @throws Not_Implemented as this operation is not possible in PKCS11
       */
      std::unique_ptr<Private_Key> generate_another(RandomNumberGenerator& /*rng*/) const override {
         throw Not_Implemented("Cannot generate a new PKCS#11 ECDH keypair from this private key");
      }

      bool supports_operation(PublicKeyOperation op) const override { return (op == PublicKeyOperation::KeyAgreement); }

      std::unique_ptr<PK_Ops::Key_Agreement> create_key_agreement_op(RandomNumberGenerator& rng,
                                                                     std::string_view params,
                                                                     std::string_view provider) const override;
};

BOTAN_DIAGNOSTIC_POP

using PKCS11_ECDH_KeyPair = std::pair<PKCS11_ECDH_PublicKey, PKCS11_ECDH_PrivateKey>;

/**
* PKCS#11 ECDH key pair generation
* @param session the session that should be used for the key generation
* @param pub_props the properties of the public key
* @param priv_props the properties of the private key
*/
BOTAN_PUBLIC_API(2, 0)
PKCS11_ECDH_KeyPair generate_ecdh_keypair(Session& session,
                                          const EC_PublicKeyGenerationProperties& pub_props,
                                          const EC_PrivateKeyGenerationProperties& priv_props);
}  // namespace Botan::PKCS11

#endif

#if defined(BOTAN_HAS_ECDSA)


   #include <string>

namespace Botan::PKCS11 {
class Session;

/// Represents a PKCS#11 ECDSA public key

BOTAN_DIAGNOSTIC_PUSH
BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE

class BOTAN_PUBLIC_API(2, 0) PKCS11_ECDSA_PublicKey final : public PKCS11_EC_PublicKey,
                                                            public virtual ECDSA_PublicKey {
   public:
      /**
      * Creates a PKCS11_ECDSA_PublicKey object from an existing PKCS#11 ECDSA public key
      * @param session the session to use
      * @param handle the handle of the ECDSA public key
      */
      PKCS11_ECDSA_PublicKey(Session& session, ObjectHandle handle) : PKCS11_EC_PublicKey(session, handle) {}

      /**
      * Imports an ECDSA public key
      * @param session the session to use
      * @param props the attributes of the public key
      */
      PKCS11_ECDSA_PublicKey(Session& session, const EC_PublicKeyImportProperties& props) :
            PKCS11_EC_PublicKey(session, props) {}

      inline std::string algo_name() const override { return "ECDSA"; }

      /// @return the exported ECDSA public key
      ECDSA_PublicKey export_key() const;

      /**
       * @throws Not_Implemented as this operation is not possible in PKCS11
       */
      std::unique_ptr<Private_Key> generate_another(RandomNumberGenerator& /*rng*/) const final {
         throw Not_Implemented("Cannot generate a new PKCS#11 ECDSA keypair from this public key");
      }

      std::unique_ptr<PK_Ops::Verification> create_verification_op(std::string_view params,
                                                                   std::string_view provider) const override;
};

BOTAN_DIAGNOSTIC_POP

/// Represents a PKCS#11 ECDSA private key
class BOTAN_PUBLIC_API(2, 0) PKCS11_ECDSA_PrivateKey final : public PKCS11_EC_PrivateKey {
   public:
      /**
      * Creates a PKCS11_ECDSA_PrivateKey object from an existing PKCS#11 ECDSA private key
      * @param session the session to use
      * @param handle the handle of the ECDSA private key
      */
      PKCS11_ECDSA_PrivateKey(Session& session, ObjectHandle handle) : PKCS11_EC_PrivateKey(session, handle) {}

      /**
      * Imports a ECDSA private key
      * @param session the session to use
      * @param props the attributes of the private key
      */
      PKCS11_ECDSA_PrivateKey(Session& session, const EC_PrivateKeyImportProperties& props) :
            PKCS11_EC_PrivateKey(session, props) {}

      /**
      * Generates a PKCS#11 ECDSA private key
      * @param session the session to use
      * @param ec_params DER-encoding of an ANSI X9.62 Parameters value
      * @param props the attributes of the private key
      * @note no persistent public key object will be created
      */
      PKCS11_ECDSA_PrivateKey(Session& session,
                              const std::vector<uint8_t>& ec_params,
                              const EC_PrivateKeyGenerationProperties& props) :
            PKCS11_EC_PrivateKey(session, ec_params, props) {}

      inline std::string algo_name() const override { return "ECDSA"; }

      /**
       * @throws Not_Implemented as this operation is not possible in PKCS11
       */
      std::unique_ptr<Private_Key> generate_another(RandomNumberGenerator& /*rng*/) const override {
         throw Not_Implemented("Cannot generate a new PKCS#11 ECDSA keypair from this private key");
      }

      bool supports_operation(PublicKeyOperation op) const override { return (op == PublicKeyOperation::Signature); }

      std::optional<size_t> _signature_element_size_for_DER_encoding() const override {
         return domain().get_order_bytes();
      }

      /// @return the exported ECDSA private key
      ECDSA_PrivateKey export_key() const;

      std::unique_ptr<Public_Key> public_key() const override;

      secure_vector<uint8_t> private_key_bits() const override;

      bool check_key(RandomNumberGenerator& rng, bool strong) const override;

      std::unique_ptr<PK_Ops::Signature> create_signature_op(RandomNumberGenerator& rng,
                                                             std::string_view params,
                                                             std::string_view provider) const override;
};

using PKCS11_ECDSA_KeyPair = std::pair<PKCS11_ECDSA_PublicKey, PKCS11_ECDSA_PrivateKey>;

/**
* ECDSA key pair generation
* @param session the session that should be used for the key generation
* @param pub_props the properties of the public key
* @param priv_props the properties of the private key
*/
BOTAN_PUBLIC_API(2, 0)
PKCS11_ECDSA_KeyPair generate_ecdsa_keypair(Session& session,
                                            const EC_PublicKeyGenerationProperties& pub_props,
                                            const EC_PrivateKeyGenerationProperties& priv_props);
}  // namespace Botan::PKCS11

#endif


namespace Botan::PKCS11 {

/**
* Simple class to build and hold the data for a CK_MECHANISM struct
* for RSA (encryption/decryption, signature/verification)
* and EC (ECDSA signature/verification, ECDH key derivation).
*/
class BOTAN_PUBLIC_API(3, 7) MechanismWrapper final {
   public:
      /// @param mechanism_type the CK_MECHANISM_TYPE for the `mechanism` field of the CK_MECHANISM struct
      explicit MechanismWrapper(MechanismType mechanism_type);

      /**
      * Creates the CK_MECHANISM data for RSA encryption/decryption
      * @param padding supported paddings are Raw (X.509), EME-PKCS1-v1_5 (PKCS#1 v1.5) and OAEP (PKCS#1 OAEP)
      */
      static MechanismWrapper create_rsa_crypt_mechanism(std::string_view padding);

      /**
      * Creates the CK_MECHANISM data for RSA signature/verification
      * @param padding supported paddings are Raw (X.509), EMSA3 (PKCS#1 v1.5), EMSA4 (PKCS#1 PSS),
      * EMSA2 (ANSI X9.31) and ISO9796 (ISO/IEC 9796)
      */
      static MechanismWrapper create_rsa_sign_mechanism(std::string_view padding);

      /**
      * Creates the CK_MECHANISM data for ECDSA signature/verification
      * @param hash the hash algorithm used to hash the data to sign.
      * supported hash functions are Raw and SHA-1 to SHA-512
      */
      static MechanismWrapper create_ecdsa_mechanism(std::string_view hash);

      /**
      * Creates the CK_MECHANISM data for ECDH key derivation (CKM_ECDH1_DERIVE or CKM_ECDH1_COFACTOR_DERIVE)
      * @param params specifies the key derivation function to use.
      * Supported KDFs are Raw and SHA-1 to SHA-512.
      * Params can also include the string "Cofactor" if the cofactor
      * key derivation mechanism should be used, for example "SHA-512,Cofactor"
      */
      static MechanismWrapper create_ecdh_mechanism(std::string_view params);

      /**
      * Sets the salt for the ECDH mechanism parameters.
      * @param salt the salt
      * @param salt_len size of the salt in bytes
      */
      inline void set_ecdh_salt(const uint8_t salt[], size_t salt_len) {
         m_parameters->ecdh_params.pSharedData = const_cast<uint8_t*>(salt);
         m_parameters->ecdh_params.ulSharedDataLen = static_cast<Ulong>(salt_len);
      }

      /**
      * Sets the public key of the other party for the ECDH mechanism parameters.
      * @param other_key key of the other party
      * @param other_key_len size of the key of the other party in bytes
      */
      inline void set_ecdh_other_key(const uint8_t other_key[], size_t other_key_len) {
         m_parameters->ecdh_params.pPublicData = const_cast<uint8_t*>(other_key);
         m_parameters->ecdh_params.ulPublicDataLen = static_cast<Ulong>(other_key_len);
      }

      /// @return a pointer to the CK_MECHANISM struct that can be passed to the cryptoki functions
      inline Mechanism* data() const { return const_cast<Mechanism*>(&m_mechanism); }

      inline MechanismType mechanism_type() const { return static_cast<MechanismType>(m_mechanism.mechanism); }

      /// @return the size of the padding in bytes (for encryption/decryption)
      inline size_t padding_size() const { return m_padding_size; }

      /// Holds the mechanism parameters for OAEP, PSS and ECDH
      ///
      /// TODO(Botan4) use a std::variant here
      /// TODO(Botan4) make this union decl private
      union MechanismParameters {
            MechanismParameters() /* NOLINT(*-member-init) */ { std::memset(this, 0, sizeof(MechanismParameters)); }

            RsaPkcsOaepParams oaep_params;
            RsaPkcsPssParams pss_params;
            Ecdh1DeriveParams ecdh_params;
      };

   private:
      Mechanism m_mechanism;
      std::shared_ptr<MechanismParameters> m_parameters;
      size_t m_padding_size = 0;
};

}  // namespace Botan::PKCS11


namespace Botan::PKCS11 {

class Module;

/// A random generator that only fetches random from the PKCS#11 RNG
class BOTAN_PUBLIC_API(2, 0) PKCS11_RNG final : public Hardware_RNG {
   public:
      /// Initialize the RNG with the PKCS#11 session that provides access to the cryptoki functions
      explicit PKCS11_RNG(Session& session);

      std::string name() const override { return "PKCS11_RNG"; }

      /// Always returns true
      bool is_seeded() const override { return true; }

      /// No operation - always returns 0
      size_t reseed_from_sources(Entropy_Sources& /*srcs*/, size_t /*bits*/) override;

      /// @return the module used by this RNG
      inline Module& module() const { return m_session.get().module(); }

      // C_SeedRandom may succeed
      bool accepts_input() const override { return true; }

   private:
      /// Calls `C_GenerateRandom` to generate random data
      /// Calls `C_SeedRandom` to add entropy to the random generation function of the token/middleware
      void fill_bytes_with_input(std::span<uint8_t> output, std::span<const uint8_t> input) override;

   private:
      const std::reference_wrapper<Session> m_session;
};
}  // namespace Botan::PKCS11

#if defined(BOTAN_HAS_RSA)
   #include <utility>

namespace Botan::PKCS11 {

/// Properties for generating a PKCS#11 RSA public key
class BOTAN_PUBLIC_API(2, 0) RSA_PublicKeyGenerationProperties final : public PublicKeyProperties {
   public:
      /// @param bits length in bits of modulus n
      explicit RSA_PublicKeyGenerationProperties(Ulong bits);

      /// @param pub_exponent public exponent e
      inline void set_pub_exponent(const BigInt& pub_exponent = BigInt::from_word(0x10001)) {
         add_binary(AttributeType::PublicExponent, pub_exponent.serialize());
      }
};

/// Properties for importing a PKCS#11 RSA public key
class BOTAN_PUBLIC_API(2, 0) RSA_PublicKeyImportProperties final : public PublicKeyProperties {
   public:
      /// @param modulus modulus n
      /// @param pub_exponent public exponent e
      RSA_PublicKeyImportProperties(const BigInt& modulus, const BigInt& pub_exponent);

      /// @return the modulus
      inline const BigInt& modulus() const { return m_modulus; }

      /// @return the public exponent
      inline const BigInt& pub_exponent() const { return m_pub_exponent; }

   private:
      const BigInt m_modulus;
      const BigInt m_pub_exponent;
};

/// Represents a PKCS#11 RSA public key
class BOTAN_PUBLIC_API(2, 0) PKCS11_RSA_PublicKey : public Object,
                                                    public RSA_PublicKey {
   public:
      static const ObjectClass Class = ObjectClass::PublicKey;

      /**
      * Creates a PKCS11_RSA_PublicKey object from an existing PKCS#11 RSA public key
      * @param session the session to use
      * @param handle the handle of the RSA public key
      */
      PKCS11_RSA_PublicKey(Session& session, ObjectHandle handle);

      /**
      * Imports a RSA public key
      * @param session the session to use
      * @param pubkey_props the attributes of the public key
      */
      PKCS11_RSA_PublicKey(Session& session, const RSA_PublicKeyImportProperties& pubkey_props);

      /**
       * @throws Not_Implemented as this operation is not possible in PKCS11
       */
      std::unique_ptr<Private_Key> generate_another(RandomNumberGenerator& /*rng*/) const final {
         throw Not_Implemented("Cannot generate a new PKCS#11 RSA keypair from this public key");
      }

      std::unique_ptr<PK_Ops::Encryption> create_encryption_op(RandomNumberGenerator& rng,
                                                               std::string_view params,
                                                               std::string_view provider) const override;

      std::unique_ptr<PK_Ops::Verification> create_verification_op(std::string_view params,
                                                                   std::string_view provider) const override;
};

/// Properties for importing a PKCS#11 RSA private key
class BOTAN_PUBLIC_API(2, 0) RSA_PrivateKeyImportProperties final : public PrivateKeyProperties {
   public:
      /**
      * @param modulus modulus n
      * @param priv_exponent private exponent d
      */
      RSA_PrivateKeyImportProperties(const BigInt& modulus, const BigInt& priv_exponent);

      /// @param pub_exponent public exponent e
      inline void set_pub_exponent(const BigInt& pub_exponent) {
         add_binary(AttributeType::PublicExponent, pub_exponent.serialize());
      }

      /// @param prime1 prime p
      inline void set_prime_1(const BigInt& prime1) { add_binary(AttributeType::Prime1, prime1.serialize()); }

      /// @param prime2 prime q
      inline void set_prime_2(const BigInt& prime2) { add_binary(AttributeType::Prime2, prime2.serialize()); }

      /// @param exp1 private exponent d modulo p-1
      inline void set_exponent_1(const BigInt& exp1) { add_binary(AttributeType::Exponent1, exp1.serialize()); }

      /// @param exp2 private exponent d modulo q-1
      inline void set_exponent_2(const BigInt& exp2) { add_binary(AttributeType::Exponent2, exp2.serialize()); }

      /// @param coeff CRT coefficient q^-1 mod p
      inline void set_coefficient(const BigInt& coeff) { add_binary(AttributeType::Coefficient, coeff.serialize()); }

      /// @return the modulus
      inline const BigInt& modulus() const { return m_modulus; }

      /// @return the private exponent
      inline const BigInt& priv_exponent() const { return m_priv_exponent; }

   private:
      const BigInt m_modulus;
      const BigInt m_priv_exponent;
};

/// Properties for generating a PKCS#11 RSA private key
class BOTAN_PUBLIC_API(2, 0) RSA_PrivateKeyGenerationProperties final : public PrivateKeyProperties {
   public:
      RSA_PrivateKeyGenerationProperties() : PrivateKeyProperties(KeyType::Rsa) {}
};

/// Represents a PKCS#11 RSA private key

BOTAN_DIAGNOSTIC_PUSH
BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE

class BOTAN_PUBLIC_API(2, 0) PKCS11_RSA_PrivateKey final : public Object,
                                                           public Private_Key,
                                                           public RSA_PublicKey {
   public:
      static const ObjectClass Class = ObjectClass::PrivateKey;

      /// Creates a PKCS11_RSA_PrivateKey object from an existing PKCS#11 RSA private key
      PKCS11_RSA_PrivateKey(Session& session, ObjectHandle handle);

      /**
      * Imports a RSA private key
      * @param session the session to use
      * @param priv_key_props the properties of the RSA private key
      */
      PKCS11_RSA_PrivateKey(Session& session, const RSA_PrivateKeyImportProperties& priv_key_props);

      /**
      * Generates a PKCS#11 RSA private key
      * @param session the session to use
      * @param bits length in bits of modulus n
      * @param priv_key_props the properties of the RSA private key
      * @note no persistent public key object will be created
      */
      PKCS11_RSA_PrivateKey(Session& session, uint32_t bits, const RSA_PrivateKeyGenerationProperties& priv_key_props);

      /// @return the exported RSA private key
      RSA_PrivateKey export_key() const;

      /**
       * If enabled, the PKCS#11 module gets to perform the raw RSA decryption
       * using a blinded ciphertext. The EME unpadding is performed in software.
       * This essenially hides the plaintext value from the PKCS#11 module.
       *
       * @param software_padding  if true, perform the unpadding in software
       */
      void set_use_software_padding(bool software_padding) { m_use_software_padding = software_padding; }

      bool uses_software_padding() const { return m_use_software_padding; }

      secure_vector<uint8_t> private_key_bits() const override;

      std::unique_ptr<Public_Key> public_key() const override;

      std::unique_ptr<PK_Ops::Decryption> create_decryption_op(RandomNumberGenerator& rng,
                                                               std::string_view params,
                                                               std::string_view provider) const override;

      std::unique_ptr<PK_Ops::Signature> create_signature_op(RandomNumberGenerator& rng,
                                                             std::string_view params,
                                                             std::string_view provider) const override;

   private:
      bool m_use_software_padding = false;
};

BOTAN_DIAGNOSTIC_POP

using PKCS11_RSA_KeyPair = std::pair<PKCS11_RSA_PublicKey, PKCS11_RSA_PrivateKey>;

/**
* RSA key pair generation
* @param session the session that should be used for the key generation
* @param pub_props properties of the public key
* @param priv_props properties of the private key
*/
BOTAN_PUBLIC_API(2, 0)
PKCS11_RSA_KeyPair generate_rsa_keypair(Session& session,
                                        const RSA_PublicKeyGenerationProperties& pub_props,
                                        const RSA_PrivateKeyGenerationProperties& priv_props);
}  // namespace Botan::PKCS11
#endif

#if defined(BOTAN_HAS_X509_CERTIFICATES)

   #include <vector>

namespace Botan::PKCS11 {

class Session;

/// Common attributes of all PKCS#11 X509 certificates
class BOTAN_PUBLIC_API(2, 0) X509_CertificateProperties final : public CertificateProperties {
   public:
      /**
      * @param subject DER-encoding of the certificate subject name
      * @param value BER-encoding of the certificate
      */
      X509_CertificateProperties(const std::vector<uint8_t>& subject, const std::vector<uint8_t>& value);

      BOTAN_FUTURE_EXPLICIT X509_CertificateProperties(const X509_Certificate& cert) :
            X509_CertificateProperties(cert.raw_subject_dn(), cert.BER_encode()) {}

      /// @param id key identifier for public/private key pair
      inline void set_id(const std::vector<uint8_t>& id) { add_binary(AttributeType::Id, id); }

      /// @param issuer DER-encoding of the certificate issuer name
      inline void set_issuer(const std::vector<uint8_t>& issuer) { add_binary(AttributeType::Issuer, issuer); }

      /// @param serial DER-encoding of the certificate serial number
      inline void set_serial(const std::vector<uint8_t>& serial) { add_binary(AttributeType::SerialNumber, serial); }

      /// @param hash hash value of the subject public key
      inline void set_subject_pubkey_hash(const std::vector<uint8_t>& hash) {
         add_binary(AttributeType::HashOfSubjectPublicKey, hash);
      }

      /// @param hash hash value of the issuer public key
      inline void set_issuer_pubkey_hash(const std::vector<uint8_t>& hash) {
         add_binary(AttributeType::HashOfIssuerPublicKey, hash);
      }

      /// @param alg defines the mechanism used to calculate `CKA_HASH_OF_SUBJECT_PUBLIC_KEY` and `CKA_HASH_OF_ISSUER_PUBLIC_KEY`
      inline void set_hash_alg(MechanismType alg) {
         add_numeric(AttributeType::NameHashAlgorithm, static_cast<Ulong>(alg));
      }

      /// @return the subject
      inline const std::vector<uint8_t>& subject() const { return m_subject; }

      /// @return the BER-encoding of the certificate
      inline const std::vector<uint8_t>& value() const { return m_value; }

   private:
      const std::vector<uint8_t> m_subject;
      const std::vector<uint8_t> m_value;
};

/// Represents a PKCS#11 X509 certificate
///
/// TODO(Botan4) remove the inheritance from X509_Certificate here
class BOTAN_PUBLIC_API(2, 0) PKCS11_X509_Certificate final : public Object,
                                                             public X509_Certificate {
   public:
      static const ObjectClass Class = ObjectClass::Certificate;

      /**
      * Create a PKCS11_X509_Certificate object from an existing PKCS#11 X509 cert
      * @param session the session to use
      * @param handle the handle of the X.509 certificate
      */
      PKCS11_X509_Certificate(Session& session, ObjectHandle handle);

      /**
      * Imports a X.509 certificate
      * @param session the session to use
      * @param props the attributes of the X.509 certificate
      */
      PKCS11_X509_Certificate(Session& session, const X509_CertificateProperties& props);
};

}  // namespace Botan::PKCS11

#endif

namespace Botan {

class RandomNumberGenerator;

/**
* Octet String
*/
class BOTAN_PUBLIC_API(2, 0) OctetString final {
   public:
      /**
      * @return size of this octet string in bytes
      */
      size_t length() const { return m_data.size(); }

      size_t size() const { return m_data.size(); }

      bool empty() const { return m_data.empty(); }

      /**
      * @return this object as a secure_vector<uint8_t>
      */
      secure_vector<uint8_t> bits_of() const { return m_data; }

      /**
      * @return start of this string
      */
      const uint8_t* begin() const { return m_data.data(); }

      /**
      * @return end of this string
      */
      const uint8_t* end() const { return begin() + m_data.size(); }

      /**
      * @return this encoded as hex
      */
      std::string to_string() const;

      /**
      * XOR the contents of another octet string into this one
      * @param other octet string
      * @return reference to this
      */
      OctetString& operator^=(const OctetString& other);

      /**
      * Force to have odd parity
      *
      * Deprecated. There is no reason to use this outside of interacting with
      * some very old or weird system which requires DES and also which do not
      * automatically ignore the parity bits.
      */
      BOTAN_DEPRECATED("Why would you need to do this") void set_odd_parity();

      /**
      * Create a new OctetString
      * @param str is a hex encoded string
      */
      explicit OctetString(std::string_view str = "");

      /**
      * Create a new random OctetString
      * @param rng is a random number generator
      * @param len is the desired length in bytes
      */
      OctetString(RandomNumberGenerator& rng, size_t len);

      /**
      * Create a new OctetString
      * @param in is an array
      * @param len is the length of in in bytes
      */
      OctetString(const uint8_t in[], size_t len);

      /**
      * Create a new OctetString
      * @param in a bytestring
      */
      explicit OctetString(std::span<const uint8_t> in) : m_data(in.begin(), in.end()) {}

      /**
      * Create a new OctetString
      * @param in a bytestring
      */
      explicit OctetString(secure_vector<uint8_t> in) : m_data(std::move(in)) {}

   private:
      secure_vector<uint8_t> m_data;
};

/**
* Compare two strings
* @param x an octet string
* @param y an octet string
* @return if x is equal to y
*/
BOTAN_PUBLIC_API(2, 0) bool operator==(const OctetString& x, const OctetString& y);

/**
* Compare two strings
* @param x an octet string
* @param y an octet string
* @return if x is not equal to y
*/
BOTAN_PUBLIC_API(2, 0) bool operator!=(const OctetString& x, const OctetString& y);

/**
* Concatenate two strings
* @param x an octet string
* @param y an octet string
* @return x concatenated with y
*/
BOTAN_PUBLIC_API(2, 0) OctetString operator+(const OctetString& x, const OctetString& y);

/**
* XOR two strings
* @param x an octet string
* @param y an octet string
* @return x XORed with y
*/
BOTAN_PUBLIC_API(2, 0) OctetString operator^(const OctetString& x, const OctetString& y);

/**
* Alternate name for octet string showing intent to use as a key
*/
using SymmetricKey = OctetString;

/**
* Alternate name for octet string showing intent to use as an IV
*/
using InitializationVector = OctetString;

}  // namespace Botan

/*
* This entire interface is deprecated. Use the interface in pwdhash.h
*/
BOTAN_DEPRECATED_HEADER("pbkdf.h")

namespace Botan {

/**
* Base class for PBKDF (password based key derivation function)
* implementations. Converts a password into a key using a salt
* and iterated hashing to make brute force attacks harder.
*
* Starting in 2.8 this functionality is also offered by PasswordHash.
*
* @warning
* This class will be removed in a future major release. Use PasswordHash
*/
class BOTAN_PUBLIC_API(2, 0) PBKDF /* NOLINT(*-special-member-functions) */ {
   public:
      /**
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to choose
      * @return a null pointer if the algo/provider combination cannot be found
      */
      BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash")
      static std::unique_ptr<PBKDF> create(std::string_view algo_spec, std::string_view provider = "");

      /**
      * Create an instance based on a name, or throw if the
      * algo/provider combination cannot be found. If provider is
      * empty then best available is chosen.
      */
      BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash")
      static std::unique_ptr<PBKDF> create_or_throw(std::string_view algo_spec, std::string_view provider = "");

      /**
      * @return list of available providers for this algorithm, empty if not available
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      /**
      * @return new instance of this same algorithm
      */
      virtual std::unique_ptr<PBKDF> new_object() const = 0;

      /**
      * @return new instance of this same algorithm
      */
      PBKDF* clone() const { return this->new_object().release(); }

      /**
      * @return name of this PBKDF
      */
      virtual std::string name() const = 0;

      virtual ~PBKDF() = default;

      /**
      * Derive a key from a passphrase for a number of iterations
      * specified by either iterations or if iterations == 0 then
      * running until msec time has elapsed.
      *
      * @param out buffer to store the derived key, must be of out_len bytes
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param iterations the number of iterations to use (use 10K or more)
      * @param msec if iterations is zero, then instead the PBKDF is
      *        run until msec milliseconds has passed.
      * @return the number of iterations performed
      */
      virtual size_t pbkdf(uint8_t out[],
                           size_t out_len,
                           std::string_view passphrase,
                           const uint8_t salt[],
                           size_t salt_len,
                           size_t iterations,
                           std::chrono::milliseconds msec) const = 0;

      /**
      * Derive a key from a passphrase for a number of iterations.
      *
      * @param out buffer to store the derived key, must be of out_len bytes
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param iterations the number of iterations to use (use 10K or more)
      */
      void pbkdf_iterations(uint8_t out[],
                            size_t out_len,
                            std::string_view passphrase,
                            const uint8_t salt[],
                            size_t salt_len,
                            size_t iterations) const;

      /**
      * Derive a key from a passphrase, running until msec time has elapsed.
      *
      * @param out buffer to store the derived key, must be of out_len bytes
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param msec if iterations is zero, then instead the PBKDF is
      *        run until msec milliseconds has passed.
      * @param iterations set to the number iterations executed
      */
      void pbkdf_timed(uint8_t out[],
                       size_t out_len,
                       std::string_view passphrase,
                       const uint8_t salt[],
                       size_t salt_len,
                       std::chrono::milliseconds msec,
                       size_t& iterations) const;

      /**
      * Derive a key from a passphrase for a number of iterations.
      *
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param iterations the number of iterations to use (use 10K or more)
      * @return the derived key
      */
      secure_vector<uint8_t> pbkdf_iterations(
         size_t out_len, std::string_view passphrase, const uint8_t salt[], size_t salt_len, size_t iterations) const;

      /**
      * Derive a key from a passphrase, running until msec time has elapsed.
      *
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param msec if iterations is zero, then instead the PBKDF is
      *        run until msec milliseconds has passed.
      * @param iterations set to the number iterations executed
      * @return the derived key
      */
      secure_vector<uint8_t> pbkdf_timed(size_t out_len,
                                         std::string_view passphrase,
                                         const uint8_t salt[],
                                         size_t salt_len,
                                         std::chrono::milliseconds msec,
                                         size_t& iterations) const;

      // Following kept for compat with 1.10:

      /**
      * Derive a key from a passphrase
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param iterations the number of iterations to use (use 10K or more)
      */
      OctetString derive_key(
         size_t out_len, std::string_view passphrase, const uint8_t salt[], size_t salt_len, size_t iterations) const {
         return OctetString(pbkdf_iterations(out_len, passphrase, salt, salt_len, iterations));
      }

      /**
      * Derive a key from a passphrase
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param iterations the number of iterations to use (use 10K or more)
      */
      template <typename Alloc>
      OctetString derive_key(size_t out_len,
                             std::string_view passphrase,
                             const std::vector<uint8_t, Alloc>& salt,
                             size_t iterations) const {
         return OctetString(pbkdf_iterations(out_len, passphrase, salt.data(), salt.size(), iterations));
      }

      /**
      * Derive a key from a passphrase
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param msec is how long to run the PBKDF
      * @param iterations is set to the number of iterations used
      */
      OctetString derive_key(size_t out_len,
                             std::string_view passphrase,
                             const uint8_t salt[],
                             size_t salt_len,
                             std::chrono::milliseconds msec,
                             size_t& iterations) const {
         return OctetString(pbkdf_timed(out_len, passphrase, salt, salt_len, msec, iterations));
      }

      /**
      * Derive a key from a passphrase using a certain amount of time
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param msec is how long to run the PBKDF
      * @param iterations is set to the number of iterations used
      */
      template <typename Alloc>
      OctetString derive_key(size_t out_len,
                             std::string_view passphrase,
                             const std::vector<uint8_t, Alloc>& salt,
                             std::chrono::milliseconds msec,
                             size_t& iterations) const {
         return OctetString(pbkdf_timed(out_len, passphrase, salt.data(), salt.size(), msec, iterations));
      }
};

/*
* Compatibility typedef
*/
typedef PBKDF S2K;

/**
* Password based key derivation function factory method
* @param algo_spec the name of the desired PBKDF algorithm
* @param provider the provider to use
* @return pointer to newly allocated object of that type
*/
BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash")
inline PBKDF* get_pbkdf(std::string_view algo_spec, std::string_view provider = "") {
   return PBKDF::create_or_throw(algo_spec, provider).release();
}

BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash") inline PBKDF* get_s2k(std::string_view algo_spec) {
   return PBKDF::create_or_throw(algo_spec).release();
}

}  // namespace Botan

#if !defined(BOTAN_IS_BEING_BUILT)
   #include <chrono>
#endif

namespace Botan {

/**
* Base class for password based key derivation functions.
*
* Converts a password into a key using a salt and iterated hashing to
* make brute force attacks harder.
*/
class BOTAN_PUBLIC_API(2, 8) PasswordHash /* NOLINT(*-special-member-functions) */ {
   public:
      virtual ~PasswordHash() = default;

      virtual std::string to_string() const = 0;

      /**
      * Most password hashes have some notion of iterations.
      */
      virtual size_t iterations() const = 0;

      /**
      * Some password hashing algorithms have a parameter which controls how
      * much memory is used. If not supported by some algorithm, returns 0.
      */
      virtual size_t memory_param() const { return 0; }

      /**
      * Some password hashing algorithms have a parallelism parameter.
      * If the algorithm does not support this notion, then the
      * function returns zero. This allows distinguishing between a
      * password hash which just does not support parallel operation,
      * vs one that does support parallel operation but which has been
      * configured to use a single lane.
      */
      virtual size_t parallelism() const { return 0; }

      /**
      * Returns an estimate of the total number of bytes required to perform this
      * key derivation.
      *
      * If this algorithm uses a small and constant amount of memory, with no
      * effort made towards being memory hard, this function returns 0.
      */
      virtual size_t total_memory_usage() const { return 0; }

      /**
      * @returns true if this password hash supports supplying a key
      */
      virtual bool supports_keyed_operation() const { return false; }

      /**
      * @returns true if this password hash supports supplying associated data
      */
      virtual bool supports_associated_data() const { return false; }

      /**
      * Hash a password into a bitstring
      *
      * Derive a key from the specified @p password and  @p salt, placing it into
      * @p out.
      *
      * @param out a span where the derived key will be placed
      * @param password the password to derive the key from
      * @param salt a randomly chosen salt
      *
      * This function is const, but is not thread safe. Different threads should
      * either use unique objects, or serialize all access.
      */
      void hash(std::span<uint8_t> out, std::string_view password, std::span<const uint8_t> salt) const {
         this->derive_key(out.data(), out.size(), password.data(), password.size(), salt.data(), salt.size());
      }

      /**
      * Hash a password into a bitstring
      *
      * Derive a key from the specified @p password, @p salt, @p
      * associated_data, and secret @p key, placing it into @p out. The
      * @p associated_data and @p key are both allowed to be empty. Currently
      * non-empty AD/key is only supported with Argon2.
      *
      * @param out a span where the derived key will be placed
      * @param password the password to derive the key from
      * @param salt a randomly chosen salt
      * @param associated_data some additional data
      * @param key a secret key
      *
      * This function is const, but is not thread safe. Different threads should
      * either use unique objects, or serialize all access.
      */
      void hash(std::span<uint8_t> out,
                std::string_view password,
                std::span<const uint8_t> salt,
                std::span<const uint8_t> associated_data,
                std::span<const uint8_t> key) const {
         this->derive_key(out.data(),
                          out.size(),
                          password.data(),
                          password.size(),
                          salt.data(),
                          salt.size(),
                          associated_data.data(),
                          associated_data.size(),
                          key.data(),
                          key.size());
      }

      /**
      * Derive a key from a password
      *
      * @param out buffer to store the derived key, must be of out_len bytes
      * @param out_len the desired length of the key to produce
      * @param password the password to derive the key from
      * @param password_len the length of password in bytes
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      *
      * This function is const, but is not thread safe. Different threads should
      * either use unique objects, or serialize all access.
      */
      virtual void derive_key(uint8_t out[],
                              size_t out_len,
                              const char* password,
                              size_t password_len,
                              const uint8_t salt[],
                              size_t salt_len) const = 0;

      /**
      * Derive a key from a password plus additional data and/or a secret key
      *
      * Currently this is only supported for Argon2. Using a non-empty AD or key
      * with other algorithms will cause a Not_Implemented exception.
      *
      * @param out buffer to store the derived key, must be of out_len bytes
      * @param out_len the desired length of the key to produce
      * @param password the password to derive the key from
      * @param password_len the length of password in bytes
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param ad some additional data
      * @param ad_len length of ad in bytes
      * @param key a secret key
      * @param key_len length of key in bytes
      *
      * This function is const, but is not thread safe. Different threads should
      * either use unique objects, or serialize all access.
      */
      virtual void derive_key(uint8_t out[],
                              size_t out_len,
                              const char* password,
                              size_t password_len,
                              const uint8_t salt[],
                              size_t salt_len,
                              const uint8_t ad[],
                              size_t ad_len,
                              const uint8_t key[],
                              size_t key_len) const;
};

class BOTAN_PUBLIC_API(2, 8) PasswordHashFamily /* NOLINT(*-special-member-functions) */ {
   public:
      /**
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to choose
      * @return a null pointer if the algo/provider combination cannot be found
      */
      static std::unique_ptr<PasswordHashFamily> create(std::string_view algo_spec, std::string_view provider = "");

      /**
      * Create an instance based on a name, or throw if the
      * algo/provider combination cannot be found. If provider is
      * empty then best available is chosen.
      */
      static std::unique_ptr<PasswordHashFamily> create_or_throw(std::string_view algo_spec,
                                                                 std::string_view provider = "");

      /**
      * @return list of available providers for this algorithm, empty if not available
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      virtual ~PasswordHashFamily() = default;

      /**
      * @return name of this PasswordHash
      */
      virtual std::string name() const = 0;

      /**
      * Return a new parameter set tuned for this machine
      *
      * Return a password hash instance tuned to run for approximately @p msec
      * milliseconds when producing an output of length @p output_length.
      * (Accuracy may vary, use the command line utility ``botan pbkdf_tune`` to
      * check.)
      *
      * The parameters will be selected to use at most @p max_memory_usage_mb
      * megabytes of memory, or if left as zero any size is allowed.
      *
      * This function works by running a short tuning loop to estimate the
      * performance of the algorithm, then scaling the parameters appropriately
      * to hit the target size. The length of time the tuning loop runs can be
      * controlled using the @p tuning_msec parameter.
      *
      * @param output_length how long the output length will be
      * @param desired_runtime_msec the desired execution time in milliseconds
      *
      * @param max_memory_usage_mb some password hash functions can use a
      * tunable amount of memory, in this case max_memory_usage limits the
      * amount of RAM the returned parameters will require, in mebibytes (2**20
      * bytes). It may require some small amount above the request. Set to nullopt
      * to place no limit at all.
      * @param tuning_msec how long to run the tuning loop
      */
      virtual std::unique_ptr<PasswordHash> tune_params(size_t output_length,
                                                        uint64_t desired_runtime_msec,
                                                        std::optional<size_t> max_memory_usage_mb = {},
                                                        uint64_t tuning_msec = 10) const = 0;

#if !defined(BOTAN_IS_BEING_BUILT)
      /**
      * Return a new parameter set tuned for this machine
      *
      * Return a password hash instance tuned to run for approximately @p msec
      * milliseconds when producing an output of length @p output_length.
      * (Accuracy may vary, use the command line utility ``botan pbkdf_tune`` to
      * check.)
      *
      * The parameters will be selected to use at most @p max_memory_usage_mb
      * megabytes of memory, or if left as zero any size is allowed.
      *
      * This function works by running a short tuning loop to estimate the
      * performance of the algorithm, then scaling the parameters appropriately
      * to hit the target size. The length of time the tuning loop runs can be
      * controlled using the @p tuning_msec parameter.
      *
      * @param output_length how long the output length will be
      * @param msec the desired execution time in milliseconds
      *
      * @param max_memory_usage_mb some password hash functions can use a
      * tunable amount of memory, in this case max_memory_usage limits the
      * amount of RAM the returned parameters will require, in mebibytes (2**20
      * bytes). It may require some small amount above the request. Set to zero
      * to place no limit at all.
      * @param tuning_msec how long to run the tuning loop
      *
      * TODO(Botan4) remove this
      */
      BOTAN_DEPRECATED("Use tune_params instead")
      std::unique_ptr<PasswordHash> tune(size_t output_length,
                                         std::chrono::milliseconds msec,
                                         size_t max_memory_usage_mb = 0,
                                         std::chrono::milliseconds tuning_msec = std::chrono::milliseconds(10)) const {
         std::optional<size_t> max_memory_opt;
         if(max_memory_usage_mb > 0) {
            max_memory_opt = max_memory_usage_mb;
         }

         return this->tune_params(output_length,
                                  static_cast<uint64_t>(msec.count()),
                                  max_memory_opt,
                                  static_cast<uint64_t>(tuning_msec.count()));
      }
#endif
      /**
      * Return some default parameter set for this PBKDF that should be good
      * enough for most users. The value returned may change over time as
      * processing power and attacks improve.
      */
      virtual std::unique_ptr<PasswordHash> default_params() const = 0;

      /**
      * Return a parameter chosen based on a rough approximation with the
      * specified iteration count. The exact value this returns for a particular
      * algorithm may change from over time. Think of it as an alternative to
      * tune, where time is expressed in terms of PBKDF2 iterations rather than
      * milliseconds.
      */
      virtual std::unique_ptr<PasswordHash> from_iterations(size_t iterations) const = 0;

      /**
      * Create a password hash using some scheme specific format. Parameters are as follows:
      * - For PBKDF2, PGP-S2K, and Bcrypt-PBKDF, i1 is iterations
      * - Scrypt uses N, r, p for i{1-3}
      * - Argon2 family uses memory (in KB), iterations, and parallelism for i{1-3}
      *
      * All unneeded parameters should be set to 0 or left blank.
      */
      virtual std::unique_ptr<PasswordHash> from_params(size_t i1, size_t i2 = 0, size_t i3 = 0) const = 0;
};

}  // namespace Botan

// Use pwdhash.h
BOTAN_FUTURE_INTERNAL_HEADER(pbkdf2.h)

namespace Botan {

BOTAN_PUBLIC_API(2, 0)
size_t pbkdf2(MessageAuthenticationCode& prf,
              uint8_t out[],
              size_t out_len,
              std::string_view passphrase,
              const uint8_t salt[],
              size_t salt_len,
              size_t iterations,
              std::chrono::milliseconds msec);

/**
* Perform PBKDF2. The prf is assumed to be keyed already.
*/
BOTAN_PUBLIC_API(2, 8)
void pbkdf2(MessageAuthenticationCode& prf,
            uint8_t out[],
            size_t out_len,
            const uint8_t salt[],
            size_t salt_len,
            size_t iterations);

/**
* PBKDF2
*/
class BOTAN_PUBLIC_API(2, 8) PBKDF2 final : public PasswordHash {
   public:
      PBKDF2(const MessageAuthenticationCode& prf, size_t iter) : m_prf(prf.new_object()), m_iterations(iter) {}

      BOTAN_DEPRECATED("For runtime tuning use PBKDF2_Family::tune_params")
      PBKDF2(const MessageAuthenticationCode& prf, size_t olen, std::chrono::milliseconds msec);

      size_t iterations() const override { return m_iterations; }

      std::string to_string() const override;

      void derive_key(uint8_t out[],
                      size_t out_len,
                      const char* password,
                      size_t password_len,
                      const uint8_t salt[],
                      size_t salt_len) const override;

   private:
      std::unique_ptr<MessageAuthenticationCode> m_prf;
      size_t m_iterations;
};

/**
* Family of PKCS #5 PBKDF2 operations
*/
class BOTAN_PUBLIC_API(2, 8) PBKDF2_Family final : public PasswordHashFamily {
   public:
      BOTAN_FUTURE_EXPLICIT PBKDF2_Family(std::unique_ptr<MessageAuthenticationCode> prf) : m_prf(std::move(prf)) {}

      std::string name() const override;

      std::unique_ptr<PasswordHash> tune_params(size_t output_len,
                                                uint64_t desired_runtime_msec,
                                                std::optional<size_t> max_memory,
                                                uint64_t tune_msec) const override;

      /**
      * Return some default parameter set for this PBKDF that should be good
      * enough for most users. The value returned may change over time as
      * processing power and attacks improve.
      */
      std::unique_ptr<PasswordHash> default_params() const override;

      std::unique_ptr<PasswordHash> from_iterations(size_t iter) const override;

      std::unique_ptr<PasswordHash> from_params(size_t iter, size_t /*unused*/, size_t /*unused*/) const override;

   private:
      std::unique_ptr<MessageAuthenticationCode> m_prf;
};

/**
* PKCS #5 PBKDF2 (old interface)
*/
class BOTAN_PUBLIC_API(2, 0) PKCS5_PBKDF2 final : public PBKDF {
   public:
      std::string name() const override;

      std::unique_ptr<PBKDF> new_object() const override;

      size_t pbkdf(uint8_t output_buf[],
                   size_t output_len,
                   std::string_view passphrase,
                   const uint8_t salt[],
                   size_t salt_len,
                   size_t iterations,
                   std::chrono::milliseconds msec) const override;

      /**
      * Create a PKCS #5 instance using the specified message auth code
      * @param mac_fn the MAC object to use as PRF
      */
      BOTAN_DEPRECATED("Use version taking unique_ptr")
      explicit PKCS5_PBKDF2(MessageAuthenticationCode* mac_fn) : m_mac(mac_fn) {}

      /**
      * Create a PKCS #5 instance using the specified message auth code
      * @param mac_fn the MAC object to use as PRF
      */
      BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash")
      explicit PKCS5_PBKDF2(std::unique_ptr<MessageAuthenticationCode> mac_fn) : m_mac(std::move(mac_fn)) {}

   private:
      std::unique_ptr<MessageAuthenticationCode> m_mac;
};

}  // namespace Botan

namespace Botan {

class DataSource;

namespace PEM_Code {

/**
* Encode some binary data in PEM format
* @param data binary data to encode
* @param data_len length of binary data in bytes
* @param label PEM label put after BEGIN and END
* @param line_width after this many characters, a new line is inserted
*/
BOTAN_PUBLIC_API(2, 0)
std::string encode(const uint8_t data[], size_t data_len, std::string_view label, size_t line_width = 64);

/**
* Encode some binary data in PEM format
* @param data binary data to encode
* @param label PEM label
* @param line_width after this many characters, a new line is inserted
*/
template <typename Alloc>
std::string encode(const std::vector<uint8_t, Alloc>& data, std::string_view label, size_t line_width = 64) {
   return encode(data.data(), data.size(), label, line_width);
}

/**
* Decode PEM data
* @param pem a datasource containing PEM encoded data
* @param label is set to the PEM label found for later inspection
*/
BOTAN_PUBLIC_API(2, 0) secure_vector<uint8_t> decode(DataSource& pem, std::string& label);

/**
* Decode PEM data
* @param pem a string containing PEM encoded data
* @param label is set to the PEM label found for later inspection
*/
BOTAN_PUBLIC_API(2, 0) secure_vector<uint8_t> decode(std::string_view pem, std::string& label);

/**
* Decode PEM data
* @param pem a datasource containing PEM encoded data
* @param label is what we expect the label to be
*/
BOTAN_PUBLIC_API(2, 0)
secure_vector<uint8_t> decode_check_label(DataSource& pem, std::string_view label);

/**
* Decode PEM data
* @param pem a string containing PEM encoded data
* @param label is what we expect the label to be
*/
BOTAN_PUBLIC_API(2, 0)
secure_vector<uint8_t> decode_check_label(std::string_view pem, std::string_view label);

/**
* Heuristic test for PEM data.
*/
BOTAN_PUBLIC_API(2, 0) bool matches(DataSource& source, std::string_view extra = "", size_t search_range = 4096);

}  // namespace PEM_Code

}  // namespace Botan

namespace Botan {

class Public_Key;
class Private_Key;
class AlgorithmIdentifier;
class EC_Group;
class RandomNumberGenerator;

BOTAN_PUBLIC_API(2, 0)
std::unique_ptr<Public_Key> load_public_key(const AlgorithmIdentifier& alg_id, std::span<const uint8_t> key_bits);

BOTAN_PUBLIC_API(2, 0)
std::unique_ptr<Private_Key> load_private_key(const AlgorithmIdentifier& alg_id, std::span<const uint8_t> key_bits);

/**
* Create a new key
* For ECC keys, algo_params specifies EC group (eg, "secp256r1")
* For DH/DSA/ElGamal keys, algo_params is DL group (eg, "modp/ietf/2048")
* For RSA, algo_params is integer keylength
* For McEliece, algo_params is n,t
* If algo_params is left empty, suitable default parameters are chosen.
*/
BOTAN_PUBLIC_API(2, 0)
std::unique_ptr<Private_Key> create_private_key(std::string_view algo_name,
                                                RandomNumberGenerator& rng,
                                                std::string_view algo_params = "",
                                                std::string_view provider = "");

/**
* Create a new ECC key
*/
BOTAN_PUBLIC_API(3, 0)
std::unique_ptr<Private_Key> create_ec_private_key(std::string_view algo_name,
                                                   const EC_Group& group,
                                                   RandomNumberGenerator& rng);

BOTAN_PUBLIC_API(2, 2)
std::vector<std::string> probe_provider_private_key(std::string_view algo_name,
                                                    const std::vector<std::string>& possible);

}  // namespace Botan
/**
* Ordinary applications should never need to include or use this
* header. It is exposed only for specialized applications which want
* to implement new versions of public key crypto without merging them
* as changes to the library. One actual example of such usage is an
* application which creates RSA signatures using a custom TPM library.
* Unless you're doing something like that, you don't need anything
* here. Instead use pubkey.h which wraps these types safely and
* provides a stable application-oriented API.
*
* Note: This header was accidentally pulled from the public API between
*       Botan 3.0.0 and 3.2.0, and then restored in 3.3.0. If you are
*       maintaining an application which used this header in Botan 2.x,
*       you should make sure to use Botan 3.3.0 or later when migrating.
*/


namespace Botan {

class RandomNumberGenerator;

}  // namespace Botan

namespace Botan::PK_Ops {

/**
* Public key encryption interface
*/
class BOTAN_UNSTABLE_API Encryption /* NOLINT(*special-member-functions) */ {
   public:
      /**
      * Encrypt a message returning the ciphertext
      */
      virtual std::vector<uint8_t> encrypt(std::span<const uint8_t> msg, RandomNumberGenerator& rng) = 0;

      /**
      * Return the maximum input size for this key
      */
      virtual size_t max_input_bits() const = 0;

      /**
      * Given the plaintext length, return an upper bound of the ciphertext
      * length for this key and padding.
      */
      virtual size_t ciphertext_length(size_t ptext_len) const = 0;

      virtual ~Encryption() = default;
};

/**
* Public key decryption interface
*/
class BOTAN_UNSTABLE_API Decryption /* NOLINT(*special-member-functions) */ {
   public:
      virtual secure_vector<uint8_t> decrypt(uint8_t& valid_mask, std::span<const uint8_t> ctext) = 0;

      virtual size_t plaintext_length(size_t ctext_len) const = 0;

      virtual ~Decryption() = default;
};

/**
* Public key signature verification interface
*/
class BOTAN_UNSTABLE_API Verification /* NOLINT(*special-member-functions) */ {
   public:
      /**
      * Add more data to the message currently being signed
      * @param input the input to be hashed/verified
      */
      virtual void update(std::span<const uint8_t> input) = 0;

      /**
      * Perform a verification operation
      * @param sig the signature to be checked with respect to the input
      */
      virtual bool is_valid_signature(std::span<const uint8_t> sig) = 0;

      /**
      * Return the hash function being used by this signer
      */
      virtual std::string hash_function() const = 0;

      virtual ~Verification() = default;
};

/**
* Public key signature creation interface
*/
class BOTAN_UNSTABLE_API Signature /* NOLINT(*special-member-functions) */ {
   public:
      /**
      * Add more data to the message currently being signed
      * @param input the input to be hashed/signed
      */
      virtual void update(std::span<const uint8_t> input) = 0;

      /**
      * Perform a signature operation
      * @param rng a random number generator
      */
      virtual std::vector<uint8_t> sign(RandomNumberGenerator& rng) = 0;

      /**
      * Return an upper bound on the length of the output signature
      */
      virtual size_t signature_length() const = 0;

      /**
      * Return an algorithm identifier associated with this signature scheme.
      *
      * Default implementation throws an exception
      */
      virtual AlgorithmIdentifier algorithm_identifier() const;

      /**
      * Return the hash function being used by this signer
      */
      virtual std::string hash_function() const = 0;

      virtual ~Signature() = default;
};

/**
* A generic key agreement operation (eg DH or ECDH)
*/
class BOTAN_UNSTABLE_API Key_Agreement /* NOLINT(*special-member-functions) */ {
   public:
      virtual secure_vector<uint8_t> agree(size_t key_len,
                                           std::span<const uint8_t> other_key,
                                           std::span<const uint8_t> salt) = 0;

      virtual size_t agreed_value_size() const = 0;

      virtual ~Key_Agreement() = default;
};

/**
* KEM (key encapsulation)
*/
class BOTAN_UNSTABLE_API KEM_Encryption /* NOLINT(*special-member-functions) */ {
   public:
      virtual void kem_encrypt(std::span<uint8_t> out_encapsulated_key,
                               std::span<uint8_t> out_shared_key,
                               RandomNumberGenerator& rng,
                               size_t desired_shared_key_len,
                               std::span<const uint8_t> salt) = 0;

      virtual size_t shared_key_length(size_t desired_shared_key_len) const = 0;

      virtual size_t encapsulated_key_length() const = 0;

      virtual ~KEM_Encryption() = default;
};

class BOTAN_UNSTABLE_API KEM_Decryption /* NOLINT(*special-member-functions) */ {
   public:
      virtual void kem_decrypt(std::span<uint8_t> out_shared_key,
                               std::span<const uint8_t> encapsulated_key,
                               size_t desired_shared_key_len,
                               std::span<const uint8_t> salt) = 0;

      virtual size_t shared_key_length(size_t desired_shared_key_len) const = 0;

      virtual size_t encapsulated_key_length() const = 0;

      virtual ~KEM_Decryption() = default;
};

}  // namespace Botan::PK_Ops

namespace Botan {

class PKCS10_Data;

class Private_Key;
class Extensions;
class X509_DN;
class AlternativeName;

/**
* PKCS #10 Certificate Request.
*/
class BOTAN_PUBLIC_API(2, 0) PKCS10_Request final : public X509_Object {
   public:
      /**
      * Get the subject public key.
      * @return subject public key
      */
      std::unique_ptr<Public_Key> subject_public_key() const;

      /**
      * Get the raw DER encoded public key.
      * @return raw DER encoded public key
      */
      const std::vector<uint8_t>& raw_public_key() const;

      /**
      * Get the subject DN.
      * @return subject DN
      */
      const X509_DN& subject_dn() const;

      /**
      * Get the subject alternative name.
      * @return subject alternative name.
      */
      const AlternativeName& subject_alt_name() const;

      /**
      * Get the key constraints for the key associated with this
      * PKCS#10 object.
      * @return key constraints
      */
      Key_Constraints constraints() const;

      /**
      * Get the extendend key constraints (if any).
      * @return extended key constraints
      */
      std::vector<OID> ex_constraints() const;

      /**
      * Find out whether this is a CA request.
      * @result true if it is a CA request, false otherwise.
      */
      bool is_CA() const;

      /**
      * Return the constraint on the path length defined in the BasicConstraints extension.
      *
      * Note this returns 0 if the extension is not set
      *
      * @return path limit
      */
      BOTAN_DEPRECATED("Use path_length_constraint") size_t path_limit() const {
         return path_length_constraint().value_or(0);
      }

      /**
      * Return the constraint on the path length defined in the BasicConstraints extension.
      *
      * @return path limit (or nullopt if not set)
      */
      std::optional<size_t> path_length_constraint() const;

      /**
      * Get the challenge password for this request
      * @return challenge password for this request
      */
      std::string challenge_password() const;

      /**
      * Get the X509v3 extensions.
      * @return X509v3 extensions
      */
      const Extensions& extensions() const;

      /**
      * Create a PKCS#10 Request from a data source.
      * @param source the data source providing the DER encoded request
      */
      explicit PKCS10_Request(DataSource& source);

#if defined(BOTAN_TARGET_OS_HAS_FILESYSTEM)
      /**
      * Create a PKCS#10 Request from a file.
      * @param filename the name of the file containing the DER or PEM
      * encoded request file
      */
      explicit PKCS10_Request(std::string_view filename);
#endif

      /**
      * Create a PKCS#10 Request from binary data.
      * @param vec a std::vector containing the DER value
      */
      explicit PKCS10_Request(const std::vector<uint8_t>& vec);

      /**
      * Create a new PKCS10 certificate request
      * @param key the key that will be included in the certificate request
      * @param subject_dn the DN to be placed in the request
      * @param extensions extensions to include in the request
      * @param hash_fn the hash function to use to create the signature
      * @param rng a random number generator
      * @param padding_scheme if set specifies the padding scheme, otherwise an
      *        algorithm-specific default is used.
      * @param challenge a challenge string to be included in the PKCS10 request,
      *        sometimes used for revocation purposes.
      */
      static PKCS10_Request create(const Private_Key& key,
                                   const X509_DN& subject_dn,
                                   const Extensions& extensions,
                                   std::string_view hash_fn,
                                   RandomNumberGenerator& rng,
                                   std::string_view padding_scheme = "",
                                   std::string_view challenge = "");

   private:
      std::string PEM_label() const override;

      std::vector<std::string> alternate_PEM_labels() const override;

      void force_decode() override;

      const PKCS10_Data& data() const;

      std::shared_ptr<PKCS10_Data> m_data;
};

}  // namespace Botan

namespace Botan {

class RandomNumberGenerator;

/**
* PKCS #8 General Exception
*/
class BOTAN_PUBLIC_API(2, 0) PKCS8_Exception final : public Decoding_Error {
   public:
      explicit PKCS8_Exception(std::string_view error) : Decoding_Error("PKCS #8", error) {}
};

/**
* This namespace contains functions for handling PKCS #8 private keys
*/
namespace PKCS8 {

/**
* BER encode a private key
* @param key the private key to encode
* @return BER encoded key
*/
inline secure_vector<uint8_t> BER_encode(const Private_Key& key) {
   return key.private_key_info();
}

/**
* Get a string containing a PEM encoded private key.
* @param key the key to encode
* @return encoded key
*/
BOTAN_PUBLIC_API(2, 0) std::string PEM_encode(const Private_Key& key);

/**
* Encrypt a key using PKCS #8 encryption
* @param key the key to encode
* @param rng the rng to use
* @param pass the password to use for encryption
* @param msec number of milliseconds to run the password derivation
* @param pbe_algo the name of the desired password-based encryption
*        algorithm; if empty ("") a reasonable (portable/secure)
*        default will be chosen.
* @return encrypted key in binary BER form
*/
BOTAN_PUBLIC_API(2, 0)
std::vector<uint8_t> BER_encode(const Private_Key& key,
                                RandomNumberGenerator& rng,
                                std::string_view pass,
                                std::chrono::milliseconds msec = std::chrono::milliseconds(300),
                                std::string_view pbe_algo = "");

/**
* Get a string containing a PEM encoded private key, encrypting it with a
* password.
* @param key the key to encode
* @param rng the rng to use
* @param pass the password to use for encryption
* @param msec number of milliseconds to run the password derivation
* @param pbe_algo the name of the desired password-based encryption
*        algorithm; if empty ("") a reasonable (portable/secure)
*        default will be chosen.
* @return encrypted key in PEM form
*/
BOTAN_PUBLIC_API(2, 0)
std::string PEM_encode(const Private_Key& key,
                       RandomNumberGenerator& rng,
                       std::string_view pass,
                       std::chrono::milliseconds msec = std::chrono::milliseconds(300),
                       std::string_view pbe_algo = "");

/**
* Encrypt a key using PKCS #8 encryption and a fixed iteration count
* @param key the key to encode
* @param rng the rng to use
* @param pass the password to use for encryption
* @param pbkdf_iter number of iterations to run PBKDF2
* @param cipher if non-empty specifies the cipher to use. CBC and GCM modes
*   are supported, for example "AES-128/CBC", "AES-256/GCM", "Serpent/CBC".
*   If empty a suitable default is chosen.
* @param pbkdf_hash if non-empty specifies the PBKDF hash function to use.
*   For example "SHA-256" or "SHA-384". If empty a suitable default is chosen.
* @return encrypted key in binary BER form
*/
BOTAN_PUBLIC_API(2, 1)
std::vector<uint8_t> BER_encode_encrypted_pbkdf_iter(const Private_Key& key,
                                                     RandomNumberGenerator& rng,
                                                     std::string_view pass,
                                                     size_t pbkdf_iter,
                                                     std::string_view cipher = "",
                                                     std::string_view pbkdf_hash = "");

/**
* Get a string containing a PEM encoded private key, encrypting it with a
* password.
* @param key the key to encode
* @param rng the rng to use
* @param pass the password to use for encryption
* @param pbkdf_iter number of iterations to run PBKDF
* @param cipher if non-empty specifies the cipher to use. CBC and GCM modes
*   are supported, for example "AES-128/CBC", "AES-256/GCM", "Serpent/CBC".
*   If empty a suitable default is chosen.
* @param pbkdf_hash if non-empty specifies the PBKDF hash function to use.
*   For example "SHA-256" or "SHA-384". If empty a suitable default is chosen.
* @return encrypted key in PEM form
*/
BOTAN_PUBLIC_API(2, 1)
std::string PEM_encode_encrypted_pbkdf_iter(const Private_Key& key,
                                            RandomNumberGenerator& rng,
                                            std::string_view pass,
                                            size_t pbkdf_iter,
                                            std::string_view cipher = "",
                                            std::string_view pbkdf_hash = "");

/**
* Encrypt a key using PKCS #8 encryption and a variable iteration count
* @param key the key to encode
* @param rng the rng to use
* @param pass the password to use for encryption
* @param pbkdf_msec how long to run PBKDF2
* @param pbkdf_iterations if non-null, set to the number of iterations used
* @param cipher if non-empty specifies the cipher to use. CBC and GCM modes
*   are supported, for example "AES-128/CBC", "AES-256/GCM", "Serpent/CBC".
*   If empty a suitable default is chosen.
* @param pbkdf_hash if non-empty specifies the PBKDF hash function to use.
*   For example "SHA-256" or "SHA-384". If empty a suitable default is chosen.
* @return encrypted key in binary BER form
*/
BOTAN_PUBLIC_API(2, 1)
std::vector<uint8_t> BER_encode_encrypted_pbkdf_msec(const Private_Key& key,
                                                     RandomNumberGenerator& rng,
                                                     std::string_view pass,
                                                     std::chrono::milliseconds pbkdf_msec,
                                                     size_t* pbkdf_iterations,
                                                     std::string_view cipher = "",
                                                     std::string_view pbkdf_hash = "");

/**
* Get a string containing a PEM encoded private key, encrypting it with a
* password.
* @param key the key to encode
* @param rng the rng to use
* @param pass the password to use for encryption
* @param pbkdf_msec how long in milliseconds to run PBKDF2
* @param pbkdf_iterations (output argument) number of iterations of PBKDF
*  that ended up being used
* @param cipher if non-empty specifies the cipher to use. CBC and GCM modes
*   are supported, for example "AES-128/CBC", "AES-256/GCM", "Serpent/CBC".
*   If empty a suitable default is chosen.
* @param pbkdf_hash if non-empty specifies the PBKDF hash function to use.
*   For example "SHA-256" or "SHA-384". If empty a suitable default is chosen.
* @return encrypted key in PEM form
*/
BOTAN_PUBLIC_API(2, 1)
std::string PEM_encode_encrypted_pbkdf_msec(const Private_Key& key,
                                            RandomNumberGenerator& rng,
                                            std::string_view pass,
                                            std::chrono::milliseconds pbkdf_msec,
                                            size_t* pbkdf_iterations,
                                            std::string_view cipher = "",
                                            std::string_view pbkdf_hash = "");

/**
* Load an encrypted key from a data source.
* @param source the data source providing the encoded key
* @param get_passphrase a function that returns passphrases
* @return loaded private key object
*/
BOTAN_PUBLIC_API(2, 3)
std::unique_ptr<Private_Key> load_key(DataSource& source, const std::function<std::string()>& get_passphrase);

/** Load an encrypted key from a data source.
* @param source the data source providing the encoded key
* @param pass the passphrase to decrypt the key
* @return loaded private key object
*/
BOTAN_PUBLIC_API(2, 3)
std::unique_ptr<Private_Key> load_key(DataSource& source, std::string_view pass);

/** Load an unencrypted key from a data source.
* @param source the data source providing the encoded key
* @return loaded private key object
*/
BOTAN_PUBLIC_API(2, 3)
std::unique_ptr<Private_Key> load_key(DataSource& source);

/**
* Load an encrypted key from memory.
* @param source the byte buffer containing the encoded key
* @param get_passphrase a function that returns passphrases
* @return loaded private key object
*/
BOTAN_PUBLIC_API(3, 0)
std::unique_ptr<Private_Key> load_key(std::span<const uint8_t> source,
                                      const std::function<std::string()>& get_passphrase);

/** Load an encrypted key from memory.
* @param source the byte buffer containing the encoded key
* @param pass the passphrase to decrypt the key
* @return loaded private key object
*/
BOTAN_PUBLIC_API(3, 0)
std::unique_ptr<Private_Key> load_key(std::span<const uint8_t> source, std::string_view pass);

/** Load an unencrypted key from memory.
* @param source the byte buffer containing the encoded key
* @return loaded private key object
*/
BOTAN_PUBLIC_API(3, 0)
std::unique_ptr<Private_Key> load_key(std::span<const uint8_t> source);

/**
* Copy an existing encoded key object.
* @param key the key to copy
* @return new copy of the key
*/
inline std::unique_ptr<Private_Key> copy_key(const Private_Key& key) {
   DataSource_Memory source(key.private_key_info());
   return PKCS8::load_key(source);
}

}  // namespace PKCS8

}  // namespace Botan

namespace Botan {

/**
* PSS parameters type
*
* Handles encoding/decoding of RSASSA-PSS-params from RFC 3447
*
* Only MGF1 is supported, and the trailer field must 1 (ie the variant
* from IEEE 1363a using a hash identifier is not supported)
*/
class BOTAN_PUBLIC_API(3, 7) PSS_Params final : public ASN1_Object {
   public:
      /**
      * Note that the only valid strings you can pass to this function
      * are values returned by SignaturePaddingScheme::name() and
      * these may change in a minor release.
      */
      static PSS_Params from_padding_name(std::string_view padding_name);

      /**
      * Note that the only valid strings you can pass to this function
      * are values returned by SignaturePaddingScheme::name() and
      * these may change in a minor release.
      */
      BOTAN_DEPRECATED("Use PSS_Params::from_padding_name")
      static PSS_Params from_emsa_name(std::string_view padding_name) {
         return PSS_Params::from_padding_name(padding_name);
      }

      PSS_Params(std::string_view hash_fn, size_t salt_len);

      /**
      * Decode an encoded RSASSA-PSS-params
      */
      BOTAN_FUTURE_EXPLICIT PSS_Params(std::span<const uint8_t> der);

      const AlgorithmIdentifier& hash_algid() const { return m_hash; }

      const AlgorithmIdentifier& mgf_algid() const { return m_mgf; }

      const AlgorithmIdentifier& mgf_hash_algid() const { return m_mgf_hash; }

      size_t salt_length() const { return m_salt_len; }

      size_t trailer_field() const { return m_trailer_field; }

      std::string hash_function() const { return hash_algid().oid().to_formatted_string(); }

      std::string mgf_function() const { return mgf_algid().oid().to_formatted_string(); }

      std::vector<uint8_t> serialize() const;

      void encode_into(DER_Encoder& to) const override;

   private:
      // We don't currently support uninitialized PSS_Params
      void decode_from(BER_Decoder& from) override;

      AlgorithmIdentifier m_hash;
      AlgorithmIdentifier m_mgf;
      AlgorithmIdentifier m_mgf_hash;
      size_t m_salt_len;
      size_t m_trailer_field;
};

}  // namespace Botan

namespace Botan {

class RandomNumberGenerator;

/**
* Public Key Encryptor
* This is the primary interface for public key encryption
*/
class BOTAN_PUBLIC_API(2, 0) PK_Encryptor {
   public:
      /**
      * Encrypt a message.
      * @param in the message as a byte array
      * @param length the length of the above byte array
      * @param rng the random number source to use
      * @return encrypted message
      */
      std::vector<uint8_t> encrypt(const uint8_t in[], size_t length, RandomNumberGenerator& rng) const {
         return enc(in, length, rng);
      }

      /**
      * Encrypt a message.
      * @param in the message
      * @param rng the random number source to use
      * @return encrypted message
      */
      std::vector<uint8_t> encrypt(std::span<const uint8_t> in, RandomNumberGenerator& rng) const {
         return enc(in.data(), in.size(), rng);
      }

      /**
      * Return the maximum allowed message size in bytes.
      * @return maximum message size in bytes
      */
      virtual size_t maximum_input_size() const = 0;

      /**
      * Return an upper bound on the ciphertext length
      */
      virtual size_t ciphertext_length(size_t ctext_len) const = 0;

      PK_Encryptor() = default;
      virtual ~PK_Encryptor() = default;

      PK_Encryptor(const PK_Encryptor&) = delete;
      PK_Encryptor& operator=(const PK_Encryptor&) = delete;

      PK_Encryptor(PK_Encryptor&&) noexcept = default;
      PK_Encryptor& operator=(PK_Encryptor&&) noexcept = default;

   private:
      virtual std::vector<uint8_t> enc(const uint8_t[], size_t, RandomNumberGenerator&) const = 0;
};

/**
* Public Key Decryptor
*/
class BOTAN_PUBLIC_API(2, 0) PK_Decryptor {
   public:
      /**
      * Decrypt a ciphertext, throwing an exception if the input
      * seems to be invalid (eg due to an accidental or malicious
      * error in the ciphertext).
      *
      * @param in the ciphertext as a byte array
      * @param length the length of the above byte array
      * @return decrypted message
      */
      secure_vector<uint8_t> decrypt(const uint8_t in[], size_t length) const;

      /**
      * Same as above, but taking a vector
      * @param in the ciphertext
      * @return decrypted message
      */
      secure_vector<uint8_t> decrypt(std::span<const uint8_t> in) const { return decrypt(in.data(), in.size()); }

      /**
      * Decrypt a ciphertext. If the ciphertext is invalid (eg due to
      * invalid padding) or is not the expected length, instead
      * returns a random string of the expected length. Use to avoid
      * oracle attacks, especially against PKCS #1 v1.5 decryption.
      */
      secure_vector<uint8_t> decrypt_or_random(const uint8_t in[],
                                               size_t length,
                                               size_t expected_pt_len,
                                               RandomNumberGenerator& rng) const;

      /**
      * Decrypt a ciphertext. If the ciphertext is invalid (eg due to
      * invalid padding) or is not the expected length, instead
      * returns a random string of the expected length. Use to avoid
      * oracle attacks, especially against PKCS #1 v1.5 decryption.
      *
      * Additionally checks (also in const time) that:
      *    contents[required_content_offsets[i]] == required_content_bytes[i]
      * for 0 <= i < required_contents
      *
      * Used for example in TLS, which encodes the client version in
      * the content bytes: if there is any timing variation the version
      * check can be used as an oracle to recover the key.
      */
      secure_vector<uint8_t> decrypt_or_random(const uint8_t in[],
                                               size_t length,
                                               size_t expected_pt_len,
                                               RandomNumberGenerator& rng,
                                               const uint8_t required_content_bytes[],
                                               const uint8_t required_content_offsets[],
                                               size_t required_contents) const;

      /**
      * Return an upper bound on the plaintext length for a particular
      * ciphertext input length
      */
      virtual size_t plaintext_length(size_t ctext_len) const = 0;

      PK_Decryptor() = default;
      virtual ~PK_Decryptor() = default;

      PK_Decryptor(const PK_Decryptor&) = delete;
      PK_Decryptor& operator=(const PK_Decryptor&) = delete;

      PK_Decryptor(PK_Decryptor&&) noexcept = default;
      PK_Decryptor& operator=(PK_Decryptor&&) noexcept = default;

   private:
      virtual secure_vector<uint8_t> do_decrypt(uint8_t& valid_mask, const uint8_t in[], size_t in_len) const = 0;
};

/**
* Public Key Signer. Use the sign_message() functions for small
* messages. Use multiple calls update() to process large messages and
* generate the signature by finally calling signature().
*/
class BOTAN_PUBLIC_API(2, 0) PK_Signer final {
   public:
      /**
      * Construct a PK Signer.
      * @param key the key to use inside this signer
      * @param rng the random generator to use
      * @param padding the padding/hash to use, eg "SHA-512" or "PSS(SHA-256)"
      * @param format the signature format to use
      * @param provider the provider to use
      */
      PK_Signer(const Private_Key& key,
                RandomNumberGenerator& rng,
                std::string_view padding,
                Signature_Format format = Signature_Format::Standard,
                std::string_view provider = "");

      ~PK_Signer();

      PK_Signer(const PK_Signer&) = delete;
      PK_Signer& operator=(const PK_Signer&) = delete;

      PK_Signer(PK_Signer&&) noexcept;
      PK_Signer& operator=(PK_Signer&&) noexcept;

      /**
      * Sign a message all in one go
      * @param in the message to sign as a byte array
      * @param length the length of the above byte array
      * @param rng the rng to use
      * @return signature
      */
      std::vector<uint8_t> sign_message(const uint8_t in[], size_t length, RandomNumberGenerator& rng) {
         this->update(in, length);
         return this->signature(rng);
      }

      /**
      * Sign a message.
      * @param in the message to sign
      * @param rng the rng to use
      * @return signature
      */
      std::vector<uint8_t> sign_message(std::span<const uint8_t> in, RandomNumberGenerator& rng) {
         return sign_message(in.data(), in.size(), rng);
      }

      /**
      * Add a message part (single byte).
      * @param in the byte to add
      */
      void update(uint8_t in) { update(&in, 1); }

      /**
      * Add a message part.
      * @param in the message part to add as a byte array
      * @param length the length of the above byte array
      */
      void update(const uint8_t in[], size_t length);

      /**
      * Add a message part.
      * @param in the message part to add
      */
      void update(std::span<const uint8_t> in) { update(in.data(), in.size()); }

      /**
      * Add a message part.
      * @param in the message part to add
      */
      void update(std::string_view in);

      /**
      * Get the signature of the so far processed message (provided by the
      * calls to update()).
      * @param rng the rng to use
      * @return signature of the total message
      */
      std::vector<uint8_t> signature(RandomNumberGenerator& rng);

      /**
      * Set the output format of the signature.
      * @param format the signature format to use
      */
      void set_output_format(Signature_Format format) { m_sig_format = format; }

      /**
      * Return an upper bound on the length of the signatures this
      * PK_Signer will produce
      */
      size_t signature_length() const;

      /**
      * Return an AlgorithmIdentifier appropriate for identifying the signature
      * method being generated by this PK_Signer. Throws an exception if this
      * is not available for the current signature scheme.
      */
      AlgorithmIdentifier algorithm_identifier() const;

      /**
      * Return the hash function which is being used to create signatures.
      * This should never return an empty string however it may return a string
      * which does not map directly to a hash function, in particular if "Raw"
      * (unhashed) encoding is being used.
      */
      std::string hash_function() const;

   private:
      std::unique_ptr<PK_Ops::Signature> m_op;
      Signature_Format m_sig_format;
      std::optional<size_t> m_sig_element_size;
};

/**
* Public Key Verifier. Use the verify_message() functions for small
* messages. Use multiple calls update() to process large messages and
* verify the signature by finally calling check_signature().
*/
class BOTAN_PUBLIC_API(2, 0) PK_Verifier final {
   public:
      /**
      * Construct a PK Verifier.
      * @param pub_key the public key to verify against
      * @param padding the padding/hash to use (eg "SHA-512" or "PSS(SHA-256)")
      * @param format the signature format to use
      * @param provider the provider to use
      */
      PK_Verifier(const Public_Key& pub_key,
                  std::string_view padding,
                  Signature_Format format = Signature_Format::Standard,
                  std::string_view provider = "");

      /**
      * Construct a PK Verifier (X.509 specific)
      *
      * This constructor will attempt to decode signature_format relative
      * to the public key provided. If they seem to be inconsistent or
      * otherwise unsupported, a Decoding_Error is thrown.
      *
      * @param pub_key the public key to verify against
      * @param signature_algorithm the supposed signature algorithm
      * @param provider the provider to use
      */
      PK_Verifier(const Public_Key& pub_key,
                  const AlgorithmIdentifier& signature_algorithm,
                  std::string_view provider = "");

      ~PK_Verifier();

      PK_Verifier(const PK_Verifier&) = delete;
      PK_Verifier& operator=(const PK_Verifier&) = delete;

      PK_Verifier(PK_Verifier&&) noexcept;
      PK_Verifier& operator=(PK_Verifier&&) noexcept;

      /**
      * Verify a signature.
      * @param msg the message that the signature belongs to, as a byte array
      * @param msg_length the length of the above byte array msg
      * @param sig the signature as a byte array
      * @param sig_length the length of the above byte array sig
      * @return true if the signature is valid
      */
      bool verify_message(const uint8_t msg[], size_t msg_length, const uint8_t sig[], size_t sig_length);

      /**
      * Verify a signature.
      * @param msg the message that the signature belongs to
      * @param sig the signature
      * @return true if the signature is valid
      */
      bool verify_message(std::span<const uint8_t> msg, std::span<const uint8_t> sig) {
         return verify_message(msg.data(), msg.size(), sig.data(), sig.size());
      }

      /**
      * Add a message part (single byte) of the message corresponding to the
      * signature to be verified.
      * @param in the byte to add
      */
      void update(uint8_t in) { update(&in, 1); }

      /**
      * Add a message part of the message corresponding to the
      * signature to be verified.
      * @param msg_part the new message part as a byte array
      * @param length the length of the above byte array
      */
      void update(const uint8_t msg_part[], size_t length);

      /**
      * Add a message part of the message corresponding to the
      * signature to be verified.
      * @param in the new message part
      */
      void update(std::span<const uint8_t> in) { update(in.data(), in.size()); }

      /**
      * Add a message part of the message corresponding to the
      * signature to be verified.
      */
      void update(std::string_view in);

      /**
      * Check the signature of the buffered message, i.e. the one build
      * by successive calls to update.
      * @param sig the signature to be verified as a byte array
      * @param length the length of the above byte array
      * @return true if the signature is valid, false otherwise
      */
      bool check_signature(const uint8_t sig[], size_t length);

      /**
      * Check the signature of the buffered message, i.e. the one build
      * by successive calls to update.
      * @param sig the signature to be verified
      * @return true if the signature is valid, false otherwise
      */
      bool check_signature(std::span<const uint8_t> sig) { return check_signature(sig.data(), sig.size()); }

      /**
      * Set the format of the signatures fed to this verifier.
      * @param format the signature format to use
      */
      BOTAN_DEPRECATED("Provide Signature_Format to the constructor") void set_input_format(Signature_Format format);

      /**
      * Return the hash function which is being used to verify signatures.
      * This should never return an empty string however it may return a string
      * which does not map directly to a hash function, in particular if "Raw"
      * (unhashed) encoding is being used.
      */
      std::string hash_function() const;

   private:
      std::unique_ptr<PK_Ops::Verification> m_op;
      Signature_Format m_sig_format;
      std::optional<size_t> m_sig_element_size;
};

/**
* Object used for key agreement
*/
class BOTAN_PUBLIC_API(2, 0) PK_Key_Agreement final {
   public:
      /**
      * Construct a PK Key Agreement.
      * @param key the key to use
      * @param rng the random generator to use
      * @param kdf name of the KDF to use (or 'Raw' for no KDF)
      * @param provider the algo provider to use (or empty for default)
      */
      PK_Key_Agreement(const Private_Key& key,
                       RandomNumberGenerator& rng,
                       std::string_view kdf,
                       std::string_view provider = "");

      ~PK_Key_Agreement();

      PK_Key_Agreement(const PK_Key_Agreement&) = delete;
      PK_Key_Agreement& operator=(const PK_Key_Agreement&) = delete;

      PK_Key_Agreement(PK_Key_Agreement&&) noexcept;
      PK_Key_Agreement& operator=(PK_Key_Agreement&&) noexcept;

      /**
      * Perform Key Agreement Operation
      * @param key_len the desired key output size (ignored if "Raw" KDF is used)
      * @param peer_key the other parties key
      * @param salt extra derivation salt
      */
      SymmetricKey derive_key(size_t key_len, std::span<const uint8_t> peer_key, std::span<const uint8_t> salt) const;

      /**
      * Perform Key Agreement Operation
      * @param key_len the desired key output size (ignored if "Raw" KDF is used)
      * @param peer_key the other parties key
      * @param peer_key_len the length of peer_key in bytes
      * @param salt extra derivation salt
      * @param salt_len the length of salt in bytes
      */
      SymmetricKey derive_key(
         size_t key_len, const uint8_t peer_key[], size_t peer_key_len, const uint8_t salt[], size_t salt_len) const {
         return this->derive_key(key_len, {peer_key, peer_key_len}, {salt, salt_len});
      }

      /**
      * Perform Key Agreement Operation
      * @param key_len the desired key output size (ignored if "Raw" KDF is used)
      * @param peer_key the other parties key
      * @param salt extra derivation salt
      * @param salt_len the length of salt in bytes
      */
      SymmetricKey derive_key(size_t key_len,
                              std::span<const uint8_t> peer_key,
                              const uint8_t salt[],
                              size_t salt_len) const {
         return derive_key(key_len, peer_key.data(), peer_key.size(), salt, salt_len);
      }

      /**
      * Perform Key Agreement Operation
      * @param key_len the desired key output size (ignored if "Raw" KDF is used)
      * @param peer_key the other parties key
      * @param peer_key_len the length of peer_key in bytes
      * @param salt extra derivation info
      */
      SymmetricKey derive_key(size_t key_len,
                              const uint8_t peer_key[],
                              size_t peer_key_len,
                              std::string_view salt = "") const;

      /**
      * Perform Key Agreement Operation
      * @param key_len the desired key output size (ignored if "Raw" KDF is used)
      * @param peer_key the other parties key
      * @param salt extra derivation info
      */
      SymmetricKey derive_key(size_t key_len, std::span<const uint8_t> peer_key, std::string_view salt = "") const;

      /**
      * Return the underlying size of the value that is agreed.
      * If derive_key is called with a length of 0 with a "Raw"
      * KDF, it will return a value of this size.
      */
      size_t agreed_value_size() const;

   private:
      std::unique_ptr<PK_Ops::Key_Agreement> m_op;
};

/**
* Encryption using a standard message recovery algorithm like RSA or
* ElGamal, paired with an encoding scheme like OAEP.
*/
class BOTAN_PUBLIC_API(2, 0) PK_Encryptor_EME final : public PK_Encryptor {
   public:
      size_t maximum_input_size() const override;

      /**
      * Construct an instance.
      * @param key the key to use inside the encryptor
      * @param rng the RNG to use
      * @param padding the message encoding scheme to use (eg "OAEP(SHA-256)")
      * @param provider the provider to use
      */
      PK_Encryptor_EME(const Public_Key& key,
                       RandomNumberGenerator& rng,
                       std::string_view padding,
                       std::string_view provider = "");

      ~PK_Encryptor_EME() override;

      PK_Encryptor_EME(const PK_Encryptor_EME&) = delete;
      PK_Encryptor_EME& operator=(const PK_Encryptor_EME&) = delete;

      PK_Encryptor_EME(PK_Encryptor_EME&&) noexcept;
      PK_Encryptor_EME& operator=(PK_Encryptor_EME&&) noexcept;

      /**
      * Return an upper bound on the ciphertext length for a particular
      * plaintext input length
      */
      size_t ciphertext_length(size_t ptext_len) const override;

   private:
      std::vector<uint8_t> enc(const uint8_t ptext[], size_t len, RandomNumberGenerator& rng) const override;

      std::unique_ptr<PK_Ops::Encryption> m_op;
};

/**
* Decryption with a padding scheme.
*
* This is typically only used with RSA
*/
class BOTAN_PUBLIC_API(2, 0) PK_Decryptor_EME final : public PK_Decryptor {
   public:
      /**
      * Construct an instance.
      * @param key the key to use inside the decryptor
      * @param rng the random generator to use
      * @param padding the padding scheme to use
      * @param provider the provider to use
      */
      PK_Decryptor_EME(const Private_Key& key,
                       RandomNumberGenerator& rng,
                       std::string_view padding,
                       std::string_view provider = "");

      size_t plaintext_length(size_t ptext_len) const override;

      ~PK_Decryptor_EME() override;

      PK_Decryptor_EME(const PK_Decryptor_EME&) = delete;
      PK_Decryptor_EME& operator=(const PK_Decryptor_EME&) = delete;

      PK_Decryptor_EME(PK_Decryptor_EME&&) noexcept;
      PK_Decryptor_EME& operator=(PK_Decryptor_EME&&) noexcept;

   private:
      secure_vector<uint8_t> do_decrypt(uint8_t& valid_mask, const uint8_t in[], size_t in_len) const override;

      std::unique_ptr<PK_Ops::Decryption> m_op;
};

/**
* Result of a key encapsulation operation.
*/
class KEM_Encapsulation final {
   public:
      KEM_Encapsulation(std::vector<uint8_t> encapsulated_shared_key, secure_vector<uint8_t> shared_key) :
            m_encapsulated_shared_key(std::move(encapsulated_shared_key)), m_shared_key(std::move(shared_key)) {}

      /**
      * @returns the encapsulated shared secret (encrypted with the public key)
      */
      const std::vector<uint8_t>& encapsulated_shared_key() const { return m_encapsulated_shared_key; }

      /**
      * @returns the plaintext shared secret
      */
      const secure_vector<uint8_t>& shared_key() const { return m_shared_key; }

      /**
       * @returns the pair (encapsulated key, key) extracted from @p kem
       */
      static std::pair<std::vector<uint8_t>, secure_vector<uint8_t>> destructure(
         KEM_Encapsulation&& kem) /* NOLINT(*param-not-moved*) */ {
         return std::make_pair(std::exchange(kem.m_encapsulated_shared_key, {}), std::exchange(kem.m_shared_key, {}));
      }

   private:
      friend class PK_KEM_Encryptor;

      KEM_Encapsulation(size_t encapsulated_size, size_t shared_key_size) :
            m_encapsulated_shared_key(encapsulated_size), m_shared_key(shared_key_size) {}

   private:
      std::vector<uint8_t> m_encapsulated_shared_key;
      secure_vector<uint8_t> m_shared_key;
};

/**
* Public Key Key Encapsulation Mechanism Encryption.
*/
class BOTAN_PUBLIC_API(2, 0) PK_KEM_Encryptor final {
   public:
      /**
      * Construct an instance.
      * @param key the key to encrypt to
      * @param kem_param additional KEM parameters
      * @param provider the provider to use
      */
      BOTAN_FUTURE_EXPLICIT PK_KEM_Encryptor(const Public_Key& key,
                                             std::string_view kem_param = "",
                                             std::string_view provider = "");

      /**
      * Construct an instance.
      * @param key the key to encrypt to
      * @param rng the RNG to use
      * @param kem_param additional KEM parameters
      * @param provider the provider to use
      */
      BOTAN_DEPRECATED("Use constructor that does not take RNG")
      PK_KEM_Encryptor(const Public_Key& key,
                       RandomNumberGenerator& rng,
                       std::string_view kem_param = "",
                       std::string_view provider = "");

      ~PK_KEM_Encryptor();

      PK_KEM_Encryptor(const PK_KEM_Encryptor&) = delete;
      PK_KEM_Encryptor& operator=(const PK_KEM_Encryptor&) = delete;

      PK_KEM_Encryptor(PK_KEM_Encryptor&&) noexcept;
      PK_KEM_Encryptor& operator=(PK_KEM_Encryptor&&) noexcept;

      /**
      * Return the length of the shared key returned by this KEM
      *
      * If this KEM was used with a KDF, then it will always return
      * exactly the desired key length, because the output of the KEM
      * will be hashed by the KDF.
      *
      * However if the KEM was used with "Raw" kdf, to request the
      * algorithmic output of the KEM directly, then the desired key
      * length will be ignored and a bytestring that depends on the
      * algorithm is returned
      *
      * @param desired_shared_key_len is the requested length
      */
      size_t shared_key_length(size_t desired_shared_key_len) const;

      /**
      * Return the length in bytes of encapsulated keys returned by this KEM
      */
      size_t encapsulated_key_length() const;

      /**
      * Generate a shared key for data encryption.
      *
      * @param rng                    the RNG to use
      * @param desired_shared_key_len desired size of the shared key in bytes for the KDF
      *                               (ignored if no KDF is used)
      * @param salt                   a salt value used in the KDF
      *                               (ignored if no KDF is used)
      *
      * @returns a struct with both the shared secret and its encapsulation
      */
      KEM_Encapsulation encrypt(RandomNumberGenerator& rng,
                                size_t desired_shared_key_len = 32,
                                std::span<const uint8_t> salt = {}) {
         std::vector<uint8_t> encapsulated_shared_key(encapsulated_key_length());
         secure_vector<uint8_t> shared_key(shared_key_length(desired_shared_key_len));

         encrypt(std::span{encapsulated_shared_key}, std::span{shared_key}, rng, desired_shared_key_len, salt);
         return KEM_Encapsulation(std::move(encapsulated_shared_key), std::move(shared_key));
      }

      /**
      * Generate a shared key for data encryption.
      * @param out_encapsulated_key   the generated encapsulated key
      * @param out_shared_key         the generated shared key
      * @param rng                    the RNG to use
      * @param desired_shared_key_len desired size of the shared key in bytes
      *                               (ignored if no KDF is used)
      * @param salt                   a salt value used in the KDF
      *                               (ignored if no KDF is used)
      */
      void encrypt(secure_vector<uint8_t>& out_encapsulated_key,
                   secure_vector<uint8_t>& out_shared_key,
                   RandomNumberGenerator& rng,
                   size_t desired_shared_key_len = 32,
                   std::span<const uint8_t> salt = {}) {
         out_encapsulated_key.resize(encapsulated_key_length());
         out_shared_key.resize(shared_key_length(desired_shared_key_len));
         encrypt(std::span{out_encapsulated_key}, std::span{out_shared_key}, rng, desired_shared_key_len, salt);
      }

      /**
      * Generate a shared key for data encryption.
      * @param out_encapsulated_key   the generated encapsulated key
      * @param out_shared_key         the generated shared key
      * @param rng                    the RNG to use
      * @param desired_shared_key_len desired size of the shared key in bytes
      *                               (ignored if no KDF is used)
      * @param salt                   a salt value used in the KDF
      *                               (ignored if no KDF is used)
      */
      void encrypt(std::span<uint8_t> out_encapsulated_key,
                   std::span<uint8_t> out_shared_key,
                   RandomNumberGenerator& rng,
                   size_t desired_shared_key_len = 32,
                   std::span<const uint8_t> salt = {});

      BOTAN_DEPRECATED("use overload with salt as std::span<>")
      void encrypt(secure_vector<uint8_t>& out_encapsulated_key,
                   secure_vector<uint8_t>& out_shared_key,
                   size_t desired_shared_key_len,
                   RandomNumberGenerator& rng,
                   const uint8_t salt[],
                   size_t salt_len) {
         this->encrypt(out_encapsulated_key, out_shared_key, rng, desired_shared_key_len, {salt, salt_len});
      }

      BOTAN_DEPRECATED("use overload where rng comes after the out-parameters")
      void encrypt(secure_vector<uint8_t>& out_encapsulated_key,
                   secure_vector<uint8_t>& out_shared_key,
                   size_t desired_shared_key_len,
                   RandomNumberGenerator& rng,
                   std::span<const uint8_t> salt = {}) {
         out_encapsulated_key.resize(encapsulated_key_length());
         out_shared_key.resize(shared_key_length(desired_shared_key_len));
         encrypt(out_encapsulated_key, out_shared_key, rng, desired_shared_key_len, salt);
      }

   private:
      std::unique_ptr<PK_Ops::KEM_Encryption> m_op;
};

/**
* Public Key Key Encapsulation Mechanism Decryption.
*/
class BOTAN_PUBLIC_API(2, 0) PK_KEM_Decryptor final {
   public:
      /**
      * Construct an instance.
      * @param key the key to use inside the decryptor
      * @param rng the RNG to use
      * @param kem_param additional KEM parameters
      * @param provider the provider to use
      */
      PK_KEM_Decryptor(const Private_Key& key,
                       RandomNumberGenerator& rng,
                       std::string_view kem_param = "",
                       std::string_view provider = "");

      ~PK_KEM_Decryptor();
      PK_KEM_Decryptor(const PK_KEM_Decryptor&) = delete;
      PK_KEM_Decryptor& operator=(const PK_KEM_Decryptor&) = delete;

      PK_KEM_Decryptor(PK_KEM_Decryptor&&) noexcept;
      PK_KEM_Decryptor& operator=(PK_KEM_Decryptor&&) noexcept;

      /**
      * Return the length of the shared key returned by this KEM
      *
      * If this KEM was used with a KDF, then it will always return
      * exactly the desired key length, because the output of the KEM
      * will be hashed by the KDF.
      *
      * However if the KEM was used with "Raw" kdf, to request the
      * algorithmic output of the KEM directly, then the desired key
      * length will be ignored and a bytestring that depends on the
      * algorithm is returned
      *
      * @param desired_shared_key_len is the requested length.
      */
      size_t shared_key_length(size_t desired_shared_key_len) const;

      /**
      * Return the length of the encapsulated key expected by this KEM
      */
      size_t encapsulated_key_length() const;

      /**
      * Decrypts the shared key for data encryption.
      *
      * @param out_shared_key         the generated shared key
      * @param encap_key              the encapsulated key
      * @param desired_shared_key_len desired size of the shared key in bytes
      *                               (ignored if no KDF is used)
      * @param salt                   a salt value used in the KDF
      *                               (ignored if no KDF is used)
      */
      void decrypt(std::span<uint8_t> out_shared_key,
                   std::span<const uint8_t> encap_key,
                   size_t desired_shared_key_len = 32,
                   std::span<const uint8_t> salt = {});

      /**
      * Decrypts the shared key for data encryption.
      *
      * @param encap_key              the encapsulated key
      * @param encap_key_len          size of the encapsulated key in bytes
      * @param desired_shared_key_len desired size of the shared key in bytes
      *                               (ignored if no KDF is used)
      * @param salt                   a salt value used in the KDF
      *                               (ignored if no KDF is used)
      * @param salt_len               size of the salt value in bytes
      *                               (ignored if no KDF is used)
      *
      * @return the shared data encryption key
      */
      secure_vector<uint8_t> decrypt(const uint8_t encap_key[],
                                     size_t encap_key_len,
                                     size_t desired_shared_key_len,
                                     const uint8_t salt[] = nullptr,
                                     size_t salt_len = 0) {
         secure_vector<uint8_t> shared_key(shared_key_length(desired_shared_key_len));
         decrypt(shared_key, {encap_key, encap_key_len}, desired_shared_key_len, {salt, salt_len});
         return shared_key;
      }

      /**
      * Decrypts the shared key for data encryption.
      *
      * @param encap_key              the encapsulated key
      * @param desired_shared_key_len desired size of the shared key in bytes
      *                               (ignored if no KDF is used)
      * @param salt                   a salt value used in the KDF
      *                               (ignored if no KDF is used)
      *
      * @return the shared data encryption key
      */
      secure_vector<uint8_t> decrypt(std::span<const uint8_t> encap_key,
                                     size_t desired_shared_key_len = 32,
                                     std::span<const uint8_t> salt = {}) {
         secure_vector<uint8_t> shared_key(shared_key_length(desired_shared_key_len));
         decrypt(shared_key, encap_key, desired_shared_key_len, salt);
         return shared_key;
      }

   private:
      std::unique_ptr<PK_Ops::KEM_Decryption> m_op;
};

}  // namespace Botan

BOTAN_DEPRECATED_HEADER("reducer.h")

namespace Botan {

/**
* Modular Reducer
*
* This class is deprecated without replacement
*/
class BOTAN_PUBLIC_API(2, 0) Modular_Reducer final {
   public:
      const BigInt& get_modulus() const { return m_modulus; }

      BigInt reduce(const BigInt& x) const;

      /**
      * Multiply mod p
      * @param x the first operand
      * @param y the second operand
      * @return (x * y) % p
      */
      BigInt multiply(const BigInt& x, const BigInt& y) const { return reduce(x * y); }

      /**
      * Multiply mod p
      * @return (x * y * z) % p
      */
      BigInt multiply(const BigInt& x, const BigInt& y, const BigInt& z) const { return multiply(x, multiply(y, z)); }

      /**
      * Square mod p
      * @param x the value to square
      * @return (x * x) % p
      */
      BigInt square(const BigInt& x) const { return reduce(x * x); }

      /**
      * Cube mod p
      * @param x the value to cube
      * @return (x * x * x) % p
      */
      BigInt cube(const BigInt& x) const { return multiply(x, this->square(x)); }

      /**
      * Low level reduction function. Mostly for internal use.
      * Sometimes useful for performance by reducing temporaries
      * Reduce x mod p and place the output in out.
      *
      * @warning X and out must not reference each other
      *
      * ws is an (ignored) a temporary workspace.
      */
      void reduce(BigInt& out, const BigInt& x, secure_vector<word>& /*ws*/) const { out = reduce(x); }

      bool initialized() const { return (m_mod_words != 0); }

      BOTAN_DEPRECATED("Use for_public_modulus or for_secret_modulus") Modular_Reducer() : m_mod_words(0) {}

      /**
      * Accepts m == 0 and leaves the Modular_Reducer in an uninitialized state
      */
      explicit Modular_Reducer(const BigInt& mod);

      /**
      * Requires that m > 0
      */
      static Modular_Reducer for_public_modulus(const BigInt& m) { return Modular_Reducer(m); }

      /**
      * Requires that m > 0
      */
      static Modular_Reducer for_secret_modulus(const BigInt& m) { return Modular_Reducer(m); }

   private:
      Modular_Reducer(const BigInt& m, BigInt mu, size_t mw) : m_modulus(m), m_mu(std::move(mu)), m_mod_words(mw) {}

      BigInt m_modulus, m_mu;
      size_t m_mod_words;
};

}  // namespace Botan


struct sqlite3;
struct sqlite3_stmt;

namespace Botan {

class BOTAN_PUBLIC_API(2, 0) Sqlite3_Database final : public SQL_Database {
   public:
      /**
       * Create a new SQLite database handle from a file.
       *
       * @param file               path to the database file be opened and/or created
       * @param sqlite_open_flags  flags that will be passed to sqlite3_open_v2()
       *                           (default: SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX)
       */
      BOTAN_FUTURE_EXPLICIT Sqlite3_Database(std::string_view file,
                                             std::optional<int> sqlite_open_flags = std::nullopt);

      ~Sqlite3_Database() override;

      Sqlite3_Database(const Sqlite3_Database& other) = delete;
      Sqlite3_Database(Sqlite3_Database&& other) = delete;
      Sqlite3_Database& operator=(const Sqlite3_Database& other) = delete;
      Sqlite3_Database& operator=(Sqlite3_Database&& other) = delete;

      size_t row_count(std::string_view table_name) override;

      void create_table(std::string_view table_schema) override;

      size_t rows_changed_by_last_statement() override;

      std::shared_ptr<Statement> new_statement(std::string_view sql) const override;

      bool is_threadsafe() const override;

   private:
      class Sqlite3_Statement final : public Statement {
         public:
            void bind(int column, std::string_view val) override;
            void bind(int column, size_t val) override;
            void bind(int column, std::chrono::system_clock::time_point time) override;
            void bind(int column, const std::vector<uint8_t>& val) override;
            void bind(int column, const uint8_t* data, size_t len) override;

            std::pair<const uint8_t*, size_t> get_blob(int column) override;
            std::string get_str(int column) override;
            size_t get_size_t(int column) override;

            size_t spin() override;
            bool step() override;

            Sqlite3_Statement(sqlite3* db, std::string_view base_sql);
            ~Sqlite3_Statement() override;

            Sqlite3_Statement(const Sqlite3_Statement& other) = delete;
            Sqlite3_Statement(Sqlite3_Statement&& other) = delete;
            Sqlite3_Statement& operator=(const Sqlite3_Statement& other) = delete;
            Sqlite3_Statement& operator=(Sqlite3_Statement&& other) = delete;

         private:
            sqlite3_stmt* m_stmt;
      };

      sqlite3* m_db;
};

}  // namespace Botan

namespace Botan {

template <typename T, typename Tag, typename... Capabilities>
class Strong;

template <typename... Ts>
struct is_strong_type : std::false_type {};

template <typename... Ts>
struct is_strong_type<Strong<Ts...>> : std::true_type {};

template <typename... Ts>
constexpr bool is_strong_type_v = is_strong_type<std::remove_const_t<Ts>...>::value;

namespace concepts {

template <typename T>
concept streamable = requires(std::ostream& os, T a) { os << a; };

template <class T>
concept strong_type = is_strong_type_v<T>;

template <class T>
concept contiguous_strong_type = strong_type<T> && contiguous_container<T>;

template <class T>
concept integral_strong_type = strong_type<T> && std::integral<typename T::wrapped_type>;

template <class T>
concept unsigned_integral_strong_type = strong_type<T> && std::unsigned_integral<typename T::wrapped_type>;

template <typename T, typename Capability>
concept strong_type_with_capability = T::template has_capability<Capability>();

}  // namespace concepts

/**
 * Added as an additional "capability tag" to enable arithmetic operators with
 * plain numbers for Strong<> types that wrap a number.
 */
struct EnableArithmeticWithPlainNumber {};

namespace detail {

/**
 * Checks whether the @p CapabilityT is included in the @p Tags type pack.
 */
template <typename CapabilityT, typename... Tags>
constexpr bool has_capability = (std::is_same_v<CapabilityT, Tags> || ...);

template <typename T>
class Strong_Base {
   private:
      T m_value;

   public:
      using wrapped_type = T;

   public:
      Strong_Base() = default;
      Strong_Base(const Strong_Base&) = default;
      Strong_Base(Strong_Base&&) noexcept = default;
      Strong_Base& operator=(const Strong_Base&) = default;
      Strong_Base& operator=(Strong_Base&&) noexcept = default;
      ~Strong_Base() = default;

      constexpr explicit Strong_Base(T v) : m_value(std::move(v)) {}

      constexpr T& get() & { return m_value; }

      constexpr const T& get() const& { return m_value; }

      constexpr T&& get() && { return std::move(m_value); }

      constexpr const T&& get() const&& { return std::move(m_value); }
};

template <typename T>
class Strong_Adapter : public Strong_Base<T> {
   public:
      using Strong_Base<T>::Strong_Base;
};

template <std::integral T>
class Strong_Adapter<T> : public Strong_Base<T> {
   public:
      using Strong_Base<T>::Strong_Base;
};

template <concepts::container T>
class Container_Strong_Adapter_Base : public Strong_Base<T> {
   public:
      using value_type = typename T::value_type;
      using size_type = typename T::size_type;
      using iterator = typename T::iterator;
      using const_iterator = typename T::const_iterator;

   public:
      using Strong_Base<T>::Strong_Base;

      explicit Container_Strong_Adapter_Base(size_t size)
         requires(concepts::resizable_container<T>)
            : Container_Strong_Adapter_Base(T(size)) {}

      template <typename InputIt>
      Container_Strong_Adapter_Base(InputIt begin, InputIt end) : Container_Strong_Adapter_Base(T(begin, end)) {}

   public:
      decltype(auto) begin() noexcept(noexcept(this->get().begin())) { return this->get().begin(); }

      decltype(auto) begin() const noexcept(noexcept(this->get().begin())) { return this->get().begin(); }

      decltype(auto) end() noexcept(noexcept(this->get().end())) { return this->get().end(); }

      decltype(auto) end() const noexcept(noexcept(this->get().end())) { return this->get().end(); }

      decltype(auto) cbegin() noexcept(noexcept(this->get().cbegin())) { return this->get().cbegin(); }

      decltype(auto) cbegin() const noexcept(noexcept(this->get().cbegin())) { return this->get().cbegin(); }

      decltype(auto) cend() noexcept(noexcept(this->get().cend())) { return this->get().cend(); }

      decltype(auto) cend() const noexcept(noexcept(this->get().cend())) { return this->get().cend(); }

      size_type size() const noexcept(noexcept(this->get().size())) { return this->get().size(); }

      bool empty() const noexcept(noexcept(this->get().empty()))
         requires(concepts::has_empty<T>)
      {
         return this->get().empty();
      }

      void resize(size_type size) noexcept(noexcept(this->get().resize(size)))
         requires(concepts::resizable_container<T>)
      {
         this->get().resize(size);
      }

      void reserve(size_type size) noexcept(noexcept(this->get().reserve(size)))
         requires(concepts::reservable_container<T>)
      {
         this->get().reserve(size);
      }

      template <typename U>
      decltype(auto) operator[](U&& i) const noexcept(noexcept(this->get().operator[](i))) {
         return this->get()[std::forward<U>(i)];
      }

      template <typename U>
      decltype(auto) operator[](U&& i) noexcept(noexcept(this->get().operator[](i))) {
         return this->get()[std::forward<U>(i)];
      }
};

template <concepts::container T>
class Strong_Adapter<T> : public Container_Strong_Adapter_Base<T> {
   public:
      using Container_Strong_Adapter_Base<T>::Container_Strong_Adapter_Base;
};

template <concepts::contiguous_container T>
class Strong_Adapter<T> : public Container_Strong_Adapter_Base<T> {
   public:
      using pointer = typename T::pointer;
      using const_pointer = typename T::const_pointer;

   public:
      using Container_Strong_Adapter_Base<T>::Container_Strong_Adapter_Base;

      explicit Strong_Adapter(std::span<const typename Container_Strong_Adapter_Base<T>::value_type> span) :
            Strong_Adapter(T(span.begin(), span.end())) {}

      // Disambiguates the usage of string literals, otherwise:
      // Strong_Adapter(std::span<>) and Strong_Adapter(const char*)
      // would be ambiguous.
      explicit Strong_Adapter(const char* str)
         requires(std::same_as<T, std::string>)
            : Strong_Adapter(std::string(str)) {}

   public:
      decltype(auto) data() noexcept(noexcept(this->get().data())) { return this->get().data(); }

      decltype(auto) data() const noexcept(noexcept(this->get().data())) { return this->get().data(); }
};

}  // namespace detail

/**
 * Strong types can be used as wrappers around common types to provide
 * compile time semantics. They usually contribute to more maintainable and
 * less error-prone code especially when dealing with function parameters.
 *
 * Internally, this provides adapters so that the wrapping strong type behaves
 * as much as the underlying type as possible and desirable.
 *
 * This implementation was inspired by:
 *   https://stackoverflow.com/a/69030899
 */
template <typename T, typename TagTypeT, typename... Capabilities>
class Strong final : public detail::Strong_Adapter<T> {
   public:
      using detail::Strong_Adapter<T>::Strong_Adapter;

      template <typename CapabilityT>
      constexpr static bool has_capability() {
         return (std::is_same_v<CapabilityT, Capabilities> || ...);
      }

   private:
      using Tag = TagTypeT;
};

/**
 * @brief Generically unwraps a strong type to its underlying type.
 *
 * If the provided type is not a strong type, it is returned as is.
 *
 * @note This is meant as a helper for generic code that needs to deal with both
 *       wrapped strong types and bare objects. Use the ordinary `get()` method
 *       if you know that you are dealing with a strong type.
 *
 * @param t  value to be unwrapped
 * @return   the unwrapped value
 */
template <typename T>
[[nodiscard]] constexpr decltype(auto) unwrap_strong_type(T&& t) {
   if constexpr(!concepts::strong_type<std::remove_cvref_t<T>>) {
      // If the parameter type isn't a strong type, return it as is.
      return std::forward<T>(t);
   } else {
      // Unwrap the strong type and return the underlying value.
      return std::forward<T>(t).get();
   }
}

/**
 * @brief Wraps a value into a caller-defined (strong) type.
 *
 * If the provided object @p t is already of type @p T, it is returned as is.
 *
 * @note This is meant as a helper for generic code that needs to deal with both
 *       wrapped strong types and bare objects. Use the ordinary constructor if
 *       you know that you are dealing with a bare value type.
 *
 * @param t  value to be wrapped
 * @return   the wrapped value
 */
template <typename T, typename ParamT>
   requires std::constructible_from<T, ParamT> ||
            (concepts::strong_type<T> && std::constructible_from<typename T::wrapped_type, ParamT>)
[[nodiscard]] constexpr decltype(auto) wrap_strong_type(ParamT&& t) {
   if constexpr(std::same_as<std::remove_cvref_t<ParamT>, T>) {
      // Noop, if the parameter type already is the desired return type.
      return std::forward<ParamT>(t);
   } else if constexpr(std::constructible_from<T, ParamT>) {
      // Implicit conversion from the parameter type to the return type.
      return T{std::forward<ParamT>(t)};
   } else {
      // Explicitly calling the wrapped type's constructor to support
      // implicit conversions on types that mark their constructors as explicit.
      static_assert(concepts::strong_type<T> && std::constructible_from<typename T::wrapped_type, ParamT>);
      return T{typename T::wrapped_type{std::forward<ParamT>(t)}};
   }
}

namespace detail {

template <typename T>
struct wrapped_type_helper {
      using type = T;
};

template <concepts::strong_type T>
struct wrapped_type_helper<T> {
      using type = typename T::wrapped_type;
};

}  // namespace detail

/**
 * @brief Extracts the wrapped type from a strong type.
 *
 * If the provided type is not a strong type, it is returned as is.
 *
 * @note This is meant as a helper for generic code that needs to deal with both
 *       wrapped strong types and bare objects. Use the ordinary `::wrapped_type`
 *       declaration if you know that you are dealing with a strong type.
 */
template <typename T>
using strong_type_wrapped_type = typename detail::wrapped_type_helper<std::remove_cvref_t<T>>::type;

template <typename T, typename... Tags>
   requires(concepts::streamable<T>)
decltype(auto) operator<<(std::ostream& os, const Strong<T, Tags...>& v) {
   return os << v.get();
}

template <typename T, typename... Tags>
   requires(std::equality_comparable<T>)
bool operator==(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs) {
   return lhs.get() == rhs.get();
}

template <typename T, typename... Tags>
   requires(std::three_way_comparable<T>)
auto operator<=>(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs) {
   return lhs.get() <=> rhs.get();
}

template <std::integral T1, std::integral T2, typename... Tags>
auto operator<=>(T1 a, Strong<T2, Tags...> b) {
   return a <=> b.get();
}

template <std::integral T1, std::integral T2, typename... Tags>
auto operator<=>(Strong<T1, Tags...> a, T2 b) {
   return a.get() <=> b;
}

template <std::integral T1, std::integral T2, typename... Tags>
auto operator==(T1 a, Strong<T2, Tags...> b) {
   return a == b.get();
}

template <std::integral T1, std::integral T2, typename... Tags>
auto operator==(Strong<T1, Tags...> a, T2 b) {
   return a.get() == b;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator+(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a + b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator+(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() + b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator+(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() + b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator-(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a - b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator-(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() - b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator-(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() - b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator*(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a * b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator*(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() * b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator*(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() * b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator/(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a / b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator/(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() / b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator/(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() / b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator^(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a ^ b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator^(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() ^ b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator^(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() ^ b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator&(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a & b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator&(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() & b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator&(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() & b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator|(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a | b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator|(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() | b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator|(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() | b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator>>(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a >> b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator>>(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() >> b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator>>(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() >> b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator<<(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a << b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator<<(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() << b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator<<(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() << b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator+=(Strong<T1, Tags...>& a, T2 b) {
   a.get() += b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator+=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() += b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator-=(Strong<T1, Tags...>& a, T2 b) {
   a.get() -= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator-=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() -= b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator*=(Strong<T1, Tags...>& a, T2 b) {
   a.get() *= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator*=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() *= b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator/=(Strong<T1, Tags...>& a, T2 b) {
   a.get() /= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator/=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() /= b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator^=(Strong<T1, Tags...>& a, T2 b) {
   a.get() ^= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator^=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() ^= b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator&=(Strong<T1, Tags...>& a, T2 b) {
   a.get() &= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator&=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() &= b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator|=(Strong<T1, Tags...>& a, T2 b) {
   a.get() |= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator|=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() |= b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator>>=(Strong<T1, Tags...>& a, T2 b) {
   a.get() >>= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator>>=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() >>= b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator<<=(Strong<T1, Tags...>& a, T2 b) {
   a.get() <<= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator<<=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() <<= b.get();
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator++(Strong<T, Tags...>& a, int) {
   auto tmp = a;
   ++a.get();
   return tmp;
}

template <std::integral T, typename... Tags>
constexpr auto operator++(Strong<T, Tags...>& a) {
   ++a.get();
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator--(Strong<T, Tags...>& a, int) {
   auto tmp = a;
   --a.get();
   return tmp;
}

template <std::integral T, typename... Tags>
constexpr auto operator--(Strong<T, Tags...>& a) {
   --a.get();
   return a;
}

/**
 * This mimics a std::span but keeps track of the strong-type information. Use
 * this when you would want to use `const Strong<...>&` as a parameter
 * declaration. In particular this allows assigning strong-type information to
 * slices of a bigger buffer without copying the bytes. E.g:
 *
 *    using Foo = Strong<std::vector<uint8_t>, Foo_>;
 *
 *    void bar(StrongSpan<Foo> foo) { ... }
 *
 *    std::vector<uint8_t> buffer;
 *    BufferSlicer slicer(buffer);
 *    bar(slicer.take<Foo>());  // This does not copy the data from buffer but
 *                              // just annotates the 'Foo' strong-type info.
 */
template <concepts::contiguous_strong_type T>
class StrongSpan final {
      using underlying_span = std::
         conditional_t<std::is_const_v<T>, std::span<const typename T::value_type>, std::span<typename T::value_type>>;

   public:
      using value_type = typename underlying_span::value_type;
      using size_type = typename underlying_span::size_type;
      using iterator = typename underlying_span::iterator;
      using pointer = typename underlying_span::pointer;
      using const_pointer = typename underlying_span::const_pointer;

      StrongSpan() = default;

      explicit StrongSpan(underlying_span span) : m_span(span) {}

      // NOLINTNEXTLINE(*-explicit-conversions)
      StrongSpan(T& strong) : m_span(strong) {}

      // Allows implicit conversion from `StrongSpan<T>` to `StrongSpan<const T>`.
      // Note that this is not bi-directional. Conversion from `StrongSpan<const T>`
      // to `StrongSpan<T>` is not allowed.
      //
      // TODO: Technically, we should be able to phrase this with a `requires std::is_const_v<T>`
      //       instead of the `std::enable_if` constructions. clang-tidy (14 or 15) doesn't seem
      //       to pick up on that (yet?). As a result, for a non-const T it assumes this to be
      //       a declaration of an ordinary copy constructor. The existence of a copy constructor
      //       is interpreted as "not cheap to copy", setting off the `performance-unnecessary-value-param` check.
      //       See also: https://github.com/randombit/botan/issues/3591
      template <concepts::contiguous_strong_type T2>
      // NOLINTNEXTLINE(*-explicit-conversions)
      StrongSpan(const StrongSpan<T2>& other)
         requires(std::is_same_v<T2, std::remove_const_t<T>>)
            : m_span(other.get()) {}

      StrongSpan(const StrongSpan& other) = default;
      StrongSpan(StrongSpan&& other) = default;
      StrongSpan& operator=(const StrongSpan& other) = default;
      StrongSpan& operator=(StrongSpan&& other) = default;

      ~StrongSpan() = default;

      /**
       * @returns the underlying std::span without any type constraints
       */
      underlying_span get() const { return m_span; }

      /**
       * @returns the underlying std::span without any type constraints
       */
      underlying_span get() { return m_span; }

      decltype(auto) data() noexcept(noexcept(this->m_span.data())) { return this->m_span.data(); }

      decltype(auto) data() const noexcept(noexcept(this->m_span.data())) { return this->m_span.data(); }

      decltype(auto) size() const noexcept(noexcept(this->m_span.size())) { return this->m_span.size(); }

      bool empty() const noexcept(noexcept(this->m_span.empty())) { return this->m_span.empty(); }

      decltype(auto) begin() noexcept(noexcept(this->m_span.begin())) { return this->m_span.begin(); }

      decltype(auto) begin() const noexcept(noexcept(this->m_span.begin())) { return this->m_span.begin(); }

      decltype(auto) end() noexcept(noexcept(this->m_span.end())) { return this->m_span.end(); }

      decltype(auto) end() const noexcept(noexcept(this->m_span.end())) { return this->m_span.end(); }

      decltype(auto) operator[](typename underlying_span::size_type i) const noexcept { return this->m_span[i]; }

   private:
      underlying_span m_span;
};

template <typename>
struct is_strong_span : std::false_type {};

template <typename T>
struct is_strong_span<StrongSpan<T>> : std::true_type {};

template <typename T>
constexpr bool is_strong_span_v = is_strong_span<T>::value;

}  // namespace Botan

namespace Botan {

/**
* Return a shared reference to a global PRNG instance provided by the
* operating system. For instance might be instantiated by /dev/urandom
* or CryptGenRandom.
*/
BOTAN_PUBLIC_API(2, 0) RandomNumberGenerator& system_rng();

/*
* Instantiable reference to the system RNG.
*/
class BOTAN_PUBLIC_API(2, 0) System_RNG final : public RandomNumberGenerator {
   public:
      std::string name() const override { return system_rng().name(); }

      bool is_seeded() const override { return system_rng().is_seeded(); }

      bool accepts_input() const override { return system_rng().accepts_input(); }

      void clear() override { system_rng().clear(); }

   protected:
      void fill_bytes_with_input(std::span<uint8_t> out, std::span<const uint8_t> in) override {
         system_rng().randomize_with_input(out, in);
      }
};

}  // namespace Botan

namespace Botan {

/*
* Get information describing the version
*/

/**
* Get a human-readable single-line string identifying the version of Botan.
* No particular format should be assumed.
* @return version string
*/
BOTAN_PUBLIC_API(2, 0) std::string version_string();

/**
* Same as version_string() except returning a pointer to a statically
* allocated string.
* @return version string
*/
BOTAN_PUBLIC_API(2, 0) const char* version_cstr();

/**
* Return a version string of the form "MAJOR.MINOR.PATCH" where
* each of the values is an integer.
*/
BOTAN_PUBLIC_API(2, 4) std::string short_version_string();

/**
* Same as version_short_string except returning a pointer to the string.
*/
BOTAN_PUBLIC_API(2, 4) const char* short_version_cstr();

/**
* Return the date this version of botan was released, in an integer of
* the form YYYYMMDD. For instance a version released on May 21, 2013
* would return the integer 20130521. If the currently running version
* is not an official release, this function will return 0 instead.
*
* @return release date, or zero if unreleased
*/
BOTAN_PUBLIC_API(2, 0) uint32_t version_datestamp();

/**
* Get the major version number.
* @return major version number
*/
BOTAN_PUBLIC_API(2, 0) uint32_t version_major();

/**
* Get the minor version number.
* @return minor version number
*/
BOTAN_PUBLIC_API(2, 0) uint32_t version_minor();

/**
* Get the patch number.
* @return patch number
*/
BOTAN_PUBLIC_API(2, 0) uint32_t version_patch();

/**
* Returns a string that is set to a revision identifier corresponding to the
* source, or `nullopt` if this could not be determined. It is set for all
* official releases, and for builds that originated from within a git checkout.
*
* @return VC revision
*/
BOTAN_PUBLIC_API(3, 8) std::optional<std::string> version_vc_revision();

/**
* Return any string that is set at build time using the `--distribution-info`
* option. It allows a packager of the library to specify any distribution-specific
* patches. If no value is given at build time, returns `nullopt`.
*
* @return distribution info
*/
BOTAN_PUBLIC_API(3, 8) std::optional<std::string> version_distribution_info();

/**
* Usable for checking that the DLL version loaded at runtime exactly matches the
* compile-time version. Call using BOTAN_VERSION_* macro values, like so:
*
* ```
* Botan::runtime_version_check(BOTAN_VERSION_MAJOR, BOTAN_VERSION_MINOR, BOTAN_VERSION_PATCH);
* ```
*
* It will return an empty string if the versions match, or otherwise an error
* message indicating the discrepancy. This only is useful in dynamic libraries,
* where it is possible to compile and run against different versions.
*/
BOTAN_PUBLIC_API(2, 0) std::string runtime_version_check(uint32_t major, uint32_t minor, uint32_t patch);

/**
* Certain build-time options, used for testing, result in a binary which is not
* safe for use in a production system. This function can be used to test for such
* a configuration at runtime.
*
* Currently these unsafe conditions include:
*
* - Unsafe fuzzer mode (--unsafe-fuzzer-mode) which intentionally disables various
*   checks in order to improve the effectiveness of fuzzing.
* - Terminate on asserts (--unsafe-terminate-on-asserts) which intentionally aborts
*   if any internal assertion failure occurs, rather than throwing an exception.
*/
BOTAN_PUBLIC_API(3, 8) bool unsafe_for_production_build();

// NOLINTBEGIN(*-macro-usage)

/*
* Macros for compile-time version checks
*
* Return a value that can be used to compare versions. The current
* (compile-time) version is available as the macro BOTAN_VERSION_CODE. For
* instance, to choose one code path for version 3.1.0 and later, and another
* code path for older releases:
*
* ```
* #if BOTAN_VERSION_CODE >= BOTAN_VERSION_CODE_FOR(3,1,0)
*    // 3.1+ code path
* #else
*    // code path for older versions
* #endif
* ```
*/
#define BOTAN_VERSION_CODE_FOR(a, b, c) (((a) << 16) | ((b) << 8) | (c))

/**
* Compare using BOTAN_VERSION_CODE_FOR, as in
*  # if BOTAN_VERSION_CODE < BOTAN_VERSION_CODE_FOR(1,8,0)
*  #    error "Botan version too old"
*  # endif
*/
#define BOTAN_VERSION_CODE BOTAN_VERSION_CODE_FOR(BOTAN_VERSION_MAJOR, BOTAN_VERSION_MINOR, BOTAN_VERSION_PATCH)

// NOLINTEND(*-macro-usage)

}  // namespace Botan

namespace Botan {

class RandomNumberGenerator;
class BigInt;
class Private_Key;
class PKCS10_Request;
class PK_Signer;

/**
* An interface capable of creating new X.509 certificates
*/
class BOTAN_PUBLIC_API(2, 0) X509_CA final {
   public:
      /**
      * Return the algorithm identifier used to identify signatures that
      * this CA will create.
      */
      const AlgorithmIdentifier& algorithm_identifier() const { return m_ca_sig_algo; }

      /**
      * Return the CA's certificate
      */
      const X509_Certificate& ca_certificate() const { return m_ca_cert; }

      /**
      * Return the hash function the CA is using to sign with
      */
      const std::string& hash_function() const { return m_hash_fn; }

      /**
      * Return the signature object this CA uses to sign with
      */
      PK_Signer& signature_op() { return *m_signer; }

      /**
      * Sign a PKCS#10 Request.
      * @param req the request to sign
      * @param rng the rng to use
      * @param not_before the starting time for the certificate
      * @param not_after the expiration time for the certificate
      * @return resulting certificate
      */
      X509_Certificate sign_request(const PKCS10_Request& req,
                                    RandomNumberGenerator& rng,
                                    const X509_Time& not_before,
                                    const X509_Time& not_after) const;

      /**
      * Sign a PKCS#10 Request.
      * @param req the request to sign
      * @param rng the rng to use
      * @param serial_number the serial number the cert will be assigned.
      * @param not_before the starting time for the certificate
      * @param not_after the expiration time for the certificate
      * @return resulting certificate
      */
      X509_Certificate sign_request(const PKCS10_Request& req,
                                    RandomNumberGenerator& rng,
                                    const BigInt& serial_number,
                                    const X509_Time& not_before,
                                    const X509_Time& not_after) const;

      /**
      * Create a new and empty CRL for this CA.
      * @param rng the random number generator to use
      * @param issue_time the issue time (typically system_clock::now)
      * @param next_update the time interval after issue_data within which
      *        a new CRL will be produced.
      * @return new CRL
      */
      X509_CRL new_crl(RandomNumberGenerator& rng,
                       std::chrono::system_clock::time_point issue_time,
                       std::chrono::seconds next_update) const;

      /**
      * Create a new CRL by with additional entries.
      * @param last_crl the last CRL of this CA to add the new entries to
      * @param new_entries contains the new CRL entries to be added to the CRL
      * @param rng the random number generator to use
      * @param issue_time the issue time (typically system_clock::now)
      * @param next_update the time interval after issue_data within which
      *        a new CRL will be produced.
      */
      X509_CRL update_crl(const X509_CRL& last_crl,
                          const std::vector<CRL_Entry>& new_entries,
                          RandomNumberGenerator& rng,
                          std::chrono::system_clock::time_point issue_time,
                          std::chrono::seconds next_update) const;

      /**
      * Create a new and empty CRL for this CA.
      * @param rng the random number generator to use
      * @param next_update the time to set in next update in seconds
      * as the offset from the current time
      * @return new CRL
      */
      X509_CRL new_crl(RandomNumberGenerator& rng, uint32_t next_update = 604800) const;

      /**
      * Create a new CRL by with additional entries.
      * @param last_crl the last CRL of this CA to add the new entries to
      * @param new_entries contains the new CRL entries to be added to the CRL
      * @param rng the random number generator to use
      * @param next_update the time to set in next update in seconds
      * as the offset from the current time
      */
      X509_CRL update_crl(const X509_CRL& last_crl,
                          const std::vector<CRL_Entry>& new_entries,
                          RandomNumberGenerator& rng,
                          uint32_t next_update = 604800) const;

      /**
      * Return the set of extensions that will be used for a certificate.
      *
      * This is a helper method that is used internally. It is also exposed
      * so you can call it directly and then modify the extensions before
      * creating a certificate using X509_CA::make_cert.
      */
      static Extensions choose_extensions(const PKCS10_Request& req,
                                          const X509_Certificate& ca_certificate,
                                          std::string_view hash_fn);

      /**
      * Interface for creating new certificates
      * @param signer a signing object
      * @param rng a random number generator
      * @param sig_algo the signature algorithm identifier
      * @param pub_key the serialized public key
      * @param not_before the start time of the certificate
      * @param not_after the end time of the certificate
      * @param issuer_dn the DN of the issuer
      * @param subject_dn the DN of the subject
      * @param extensions an optional list of certificate extensions
      * @returns newly minted certificate
      */
      static X509_Certificate make_cert(PK_Signer& signer,
                                        RandomNumberGenerator& rng,
                                        const AlgorithmIdentifier& sig_algo,
                                        const std::vector<uint8_t>& pub_key,
                                        const X509_Time& not_before,
                                        const X509_Time& not_after,
                                        const X509_DN& issuer_dn,
                                        const X509_DN& subject_dn,
                                        const Extensions& extensions);

      /**
      * Interface for creating new certificates
      * @param signer a signing object
      * @param rng a random number generator
      * @param serial_number the serial number the cert will be assigned
      * @param sig_algo the signature algorithm identifier
      * @param pub_key the serialized public key
      * @param not_before the start time of the certificate
      * @param not_after the end time of the certificate
      * @param issuer_dn the DN of the issuer
      * @param subject_dn the DN of the subject
      * @param extensions an optional list of certificate extensions
      * @returns newly minted certificate
      */
      static X509_Certificate make_cert(PK_Signer& signer,
                                        RandomNumberGenerator& rng,
                                        const BigInt& serial_number,
                                        const AlgorithmIdentifier& sig_algo,
                                        const std::vector<uint8_t>& pub_key,
                                        const X509_Time& not_before,
                                        const X509_Time& not_after,
                                        const X509_DN& issuer_dn,
                                        const X509_DN& subject_dn,
                                        const Extensions& extensions);

      /**
      * Create a new CA object with custom padding option
      *
      * This is mostly useful for creating RSA-PSS certificates
      *
      * @param ca_certificate the certificate of the CA
      * @param key the private key of the CA
      * @param hash_fn name of a hash function to use for signing
      * @param padding_method name of the signature padding method to use
      * @param rng the random generator to use
      */
      X509_CA(const X509_Certificate& ca_certificate,
              const Private_Key& key,
              std::string_view hash_fn,
              std::string_view padding_method,
              RandomNumberGenerator& rng);

      /**
      * Create a new CA object.
      * @param ca_certificate the certificate of the CA
      * @param key the private key of the CA
      * @param hash_fn name of a hash function to use for signing
      * @param rng the random generator to use
      */
      X509_CA(const X509_Certificate& ca_certificate,
              const Private_Key& key,
              std::string_view hash_fn,
              RandomNumberGenerator& rng) :
            X509_CA(ca_certificate, key, hash_fn, "", rng) {}

      /**
      * Create a new CA object.
      * @param ca_certificate the certificate of the CA
      * @param key the private key of the CA
      * @param opts additional options, e.g. padding, as key value pairs
      * @param hash_fn name of a hash function to use for signing
      * @param rng the random generator to use
      */
      BOTAN_DEPRECATED("Use version taking padding as an explicit arg")

      X509_CA(const X509_Certificate& ca_certificate,
              const Private_Key& key,
              const std::map<std::string, std::string>& opts,
              std::string_view hash_fn,
              RandomNumberGenerator& rng) :
            X509_CA(ca_certificate, key, hash_fn, opts.at("padding"), rng) {}

      X509_CA(const X509_CA&) = delete;
      X509_CA& operator=(const X509_CA&) = delete;

      X509_CA(X509_CA&&) noexcept;
      X509_CA& operator=(X509_CA&&) noexcept;

      ~X509_CA();

   public:
      X509_CRL make_crl(const std::vector<CRL_Entry>& entries,
                        uint32_t crl_number,
                        RandomNumberGenerator& rng,
                        std::chrono::system_clock::time_point issue_time,
                        std::chrono::seconds next_update) const;

   private:

      AlgorithmIdentifier m_ca_sig_algo;
      X509_Certificate m_ca_cert;
      std::string m_hash_fn;
      std::unique_ptr<PK_Signer> m_signer;
};

}  // namespace Botan


namespace Botan {

class X509_Certificate;

namespace Cert_Extension {

static const size_t NO_CERT_PATH_LIMIT = 0xFFFFFFF0;

/**
* Basic Constraints Extension
*/
class BOTAN_PUBLIC_API(2, 0) Basic_Constraints final : public Certificate_Extension {
   public:
      std::unique_ptr<Certificate_Extension> copy() const override {
         return std::make_unique<Basic_Constraints>(m_is_ca, m_path_length_constraint);
      }

      BOTAN_FUTURE_EXPLICIT Basic_Constraints(bool is_ca = false, size_t path_length_constraint = 0);

      Basic_Constraints(bool is_ca, std::optional<size_t> path_length_constraint);

      BOTAN_DEPRECATED("Use is_ca") bool get_is_ca() const { return m_is_ca; }

      /**
      * Note that this function returns NO_CERT_PATH_LIMIT if the value was not set
      * in the extension.
      */
      BOTAN_DEPRECATED("Use path_length_constraint") size_t get_path_limit() const;

      bool is_ca() const { return m_is_ca; }

      std::optional<size_t> path_length_constraint() const { return m_path_length_constraint; }

      static OID static_oid() { return OID({2, 5, 29, 19}); }

      OID oid_of() const override { return static_oid(); }

   private:
      std::string oid_name() const override { return "X509v3.BasicConstraints"; }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      bool m_is_ca;
      std::optional<size_t> m_path_length_constraint;
};

/**
* Key Usage Constraints Extension
*/
class BOTAN_PUBLIC_API(2, 0) Key_Usage final : public Certificate_Extension {
   public:
      std::unique_ptr<Certificate_Extension> copy() const override {
         return std::make_unique<Key_Usage>(m_constraints);
      }

      explicit Key_Usage(Key_Constraints c) : m_constraints(c) {}

      explicit Key_Usage() : m_constraints(Key_Constraints::None) {}

      Key_Constraints get_constraints() const { return m_constraints; }

      static OID static_oid() { return OID({2, 5, 29, 15}); }

      OID oid_of() const override { return static_oid(); }

   private:
      std::string oid_name() const override { return "X509v3.KeyUsage"; }

      bool should_encode() const override { return !m_constraints.empty(); }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      Key_Constraints m_constraints;
};

/**
* Subject Key Identifier Extension
*/
class BOTAN_PUBLIC_API(2, 0) Subject_Key_ID final : public Certificate_Extension {
   public:
      Subject_Key_ID() = default;

      explicit Subject_Key_ID(const std::vector<uint8_t>& k) : m_key_id(k) {}

      Subject_Key_ID(const std::vector<uint8_t>& public_key, std::string_view hash_fn);

      std::unique_ptr<Certificate_Extension> copy() const override {
         return std::make_unique<Subject_Key_ID>(m_key_id);
      }

      const std::vector<uint8_t>& get_key_id() const { return m_key_id; }

      static OID static_oid() { return OID({2, 5, 29, 14}); }

      OID oid_of() const override { return static_oid(); }

   private:
      std::string oid_name() const override { return "X509v3.SubjectKeyIdentifier"; }

      bool should_encode() const override { return (!m_key_id.empty()); }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      std::vector<uint8_t> m_key_id;
};

/**
* Authority Key Identifier Extension
*/
class BOTAN_PUBLIC_API(2, 0) Authority_Key_ID final : public Certificate_Extension {
   public:
      std::unique_ptr<Certificate_Extension> copy() const override {
         return std::make_unique<Authority_Key_ID>(m_key_id);
      }

      Authority_Key_ID() = default;

      explicit Authority_Key_ID(const std::vector<uint8_t>& k) : m_key_id(k) {}

      const std::vector<uint8_t>& get_key_id() const { return m_key_id; }

      static OID static_oid() { return OID({2, 5, 29, 35}); }

      OID oid_of() const override { return static_oid(); }

   private:
      std::string oid_name() const override { return "X509v3.AuthorityKeyIdentifier"; }

      bool should_encode() const override { return (!m_key_id.empty()); }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      std::vector<uint8_t> m_key_id;
};

/**
* Subject Alternative Name Extension
*/
class BOTAN_PUBLIC_API(2, 4) Subject_Alternative_Name final : public Certificate_Extension {
   public:
      const AlternativeName& get_alt_name() const { return m_alt_name; }

      static OID static_oid() { return OID({2, 5, 29, 17}); }

      OID oid_of() const override { return static_oid(); }

      std::unique_ptr<Certificate_Extension> copy() const override {
         return std::make_unique<Subject_Alternative_Name>(get_alt_name());
      }

      explicit Subject_Alternative_Name(const AlternativeName& name = AlternativeName()) : m_alt_name(name) {}

   private:
      std::string oid_name() const override { return "X509v3.SubjectAlternativeName"; }

      bool should_encode() const override { return m_alt_name.has_items(); }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      AlternativeName m_alt_name;
};

/**
* Issuer Alternative Name Extension
*/
class BOTAN_PUBLIC_API(2, 0) Issuer_Alternative_Name final : public Certificate_Extension {
   public:
      const AlternativeName& get_alt_name() const { return m_alt_name; }

      static OID static_oid() { return OID({2, 5, 29, 18}); }

      OID oid_of() const override { return static_oid(); }

      std::unique_ptr<Certificate_Extension> copy() const override {
         return std::make_unique<Issuer_Alternative_Name>(get_alt_name());
      }

      explicit Issuer_Alternative_Name(const AlternativeName& name = AlternativeName()) : m_alt_name(name) {}

   private:
      std::string oid_name() const override { return "X509v3.IssuerAlternativeName"; }

      bool should_encode() const override { return m_alt_name.has_items(); }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      AlternativeName m_alt_name;
};

/**
* Extended Key Usage Extension
*/
class BOTAN_PUBLIC_API(2, 0) Extended_Key_Usage final : public Certificate_Extension {
   public:
      std::unique_ptr<Certificate_Extension> copy() const override {
         return std::make_unique<Extended_Key_Usage>(m_oids);
      }

      Extended_Key_Usage() = default;

      explicit Extended_Key_Usage(const std::vector<OID>& o) : m_oids(o) {}

      const std::vector<OID>& object_identifiers() const { return m_oids; }

      static OID static_oid() { return OID({2, 5, 29, 37}); }

      OID oid_of() const override { return static_oid(); }

   private:
      std::string oid_name() const override { return "X509v3.ExtendedKeyUsage"; }

      bool should_encode() const override { return (!m_oids.empty()); }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      std::vector<OID> m_oids;
};

/**
* Name Constraints
*/
class BOTAN_PUBLIC_API(2, 0) Name_Constraints final : public Certificate_Extension {
   public:
      std::unique_ptr<Certificate_Extension> copy() const override {
         return std::make_unique<Name_Constraints>(m_name_constraints);
      }

      Name_Constraints() = default;

      BOTAN_FUTURE_EXPLICIT Name_Constraints(const NameConstraints& nc) : m_name_constraints(nc) {}

      void validate(const X509_Certificate& subject,
                    const std::optional<X509_Certificate>& issuer,
                    const std::vector<X509_Certificate>& cert_path,
                    std::vector<std::set<Certificate_Status_Code>>& cert_status,
                    size_t pos) override;

      const NameConstraints& get_name_constraints() const { return m_name_constraints; }

      static OID static_oid() { return OID({2, 5, 29, 30}); }

      OID oid_of() const override { return static_oid(); }

   private:
      std::string oid_name() const override { return "X509v3.NameConstraints"; }

      bool should_encode() const override { return true; }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      NameConstraints m_name_constraints;
};

/**
* Certificate Policies Extension
*/
class BOTAN_PUBLIC_API(2, 0) Certificate_Policies final : public Certificate_Extension {
   public:
      std::unique_ptr<Certificate_Extension> copy() const override {
         return std::make_unique<Certificate_Policies>(m_oids);
      }

      Certificate_Policies() = default;

      explicit Certificate_Policies(const std::vector<OID>& o) : m_oids(o) {}

      const std::vector<OID>& get_policy_oids() const { return m_oids; }

      static OID static_oid() { return OID({2, 5, 29, 32}); }

      OID oid_of() const override { return static_oid(); }

      void validate(const X509_Certificate& subject,
                    const std::optional<X509_Certificate>& issuer,
                    const std::vector<X509_Certificate>& cert_path,
                    std::vector<std::set<Certificate_Status_Code>>& cert_status,
                    size_t pos) override;

   private:
      std::string oid_name() const override { return "X509v3.CertificatePolicies"; }

      bool should_encode() const override { return (!m_oids.empty()); }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      std::vector<OID> m_oids;
};

/**
* Authority Information Access Extension
*/
class BOTAN_PUBLIC_API(2, 0) Authority_Information_Access final : public Certificate_Extension {
   public:
      std::unique_ptr<Certificate_Extension> copy() const override {
         return std::make_unique<Authority_Information_Access>(m_ocsp_responders, m_ca_issuers);
      }

      Authority_Information_Access() = default;

      BOTAN_DEPRECATED("Use constructor with list of OCSP responders")
      explicit Authority_Information_Access(std::string_view ocsp,
                                            const std::vector<std::string>& ca_issuers = std::vector<std::string>()) :
            m_ocsp_responders{std::string(ocsp)}, m_ca_issuers(ca_issuers) {}

      explicit Authority_Information_Access(std::vector<std::string> ocsp_responders,
                                            std::vector<std::string> ca_issuers = std::vector<std::string>()) :
            m_ocsp_responders(std::move(ocsp_responders)), m_ca_issuers(std::move(ca_issuers)) {}

      BOTAN_DEPRECATED("Use ocsp_responders") std::string ocsp_responder() const {
         if(m_ocsp_responders.empty()) {
            return {};
         }
         return m_ocsp_responders[0];
      }

      const std::vector<std::string>& ocsp_responders() const { return m_ocsp_responders; }

      static OID static_oid() { return OID({1, 3, 6, 1, 5, 5, 7, 1, 1}); }

      OID oid_of() const override { return static_oid(); }

      const std::vector<std::string>& ca_issuers() const { return m_ca_issuers; }

   private:
      std::string oid_name() const override { return "PKIX.AuthorityInformationAccess"; }

      bool should_encode() const override { return (!m_ocsp_responders.empty() || !m_ca_issuers.empty()); }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      std::vector<std::string> m_ocsp_responders;
      std::vector<std::string> m_ca_issuers;
};

/**
* CRL Number Extension
*/
class BOTAN_PUBLIC_API(2, 0) CRL_Number final : public Certificate_Extension {
   public:
      std::unique_ptr<Certificate_Extension> copy() const override;

      CRL_Number() : m_has_value(false), m_crl_number(0) {}

      BOTAN_FUTURE_EXPLICIT CRL_Number(size_t n) : m_has_value(true), m_crl_number(n) {}

      size_t get_crl_number() const;

      static OID static_oid() { return OID({2, 5, 29, 20}); }

      OID oid_of() const override { return static_oid(); }

   private:
      std::string oid_name() const override { return "X509v3.CRLNumber"; }

      bool should_encode() const override { return m_has_value; }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      bool m_has_value;
      size_t m_crl_number;
};

/**
* CRL Entry Reason Code Extension
*/
class BOTAN_PUBLIC_API(2, 0) CRL_ReasonCode final : public Certificate_Extension {
   public:
      std::unique_ptr<Certificate_Extension> copy() const override {
         return std::make_unique<CRL_ReasonCode>(m_reason);
      }

      explicit CRL_ReasonCode(CRL_Code r = CRL_Code::Unspecified) : m_reason(r) {}

      CRL_Code get_reason() const { return m_reason; }

      static OID static_oid() { return OID({2, 5, 29, 21}); }

      OID oid_of() const override { return static_oid(); }

   private:
      std::string oid_name() const override { return "X509v3.ReasonCode"; }

      bool should_encode() const override { return (m_reason != CRL_Code::Unspecified); }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      CRL_Code m_reason;
};

/**
* CRL Distribution Points Extension
* todo enforce restrictions from RFC 5280 4.2.1.13
*/
class BOTAN_PUBLIC_API(2, 0) CRL_Distribution_Points final : public Certificate_Extension {
   public:
      class BOTAN_PUBLIC_API(2, 0) Distribution_Point final : public ASN1_Object {
         public:
            void encode_into(DER_Encoder& to) const override;
            void decode_from(BER_Decoder& from) override;

            explicit Distribution_Point(const AlternativeName& name = AlternativeName()) : m_point(name) {}

            const AlternativeName& point() const { return m_point; }

         private:
            AlternativeName m_point;
      };

      std::unique_ptr<Certificate_Extension> copy() const override {
         return std::make_unique<CRL_Distribution_Points>(m_distribution_points);
      }

      CRL_Distribution_Points() = default;

      explicit CRL_Distribution_Points(const std::vector<Distribution_Point>& points) : m_distribution_points(points) {}

      const std::vector<Distribution_Point>& distribution_points() const { return m_distribution_points; }

      const std::vector<std::string>& crl_distribution_urls() const { return m_crl_distribution_urls; }

      static OID static_oid() { return OID({2, 5, 29, 31}); }

      OID oid_of() const override { return static_oid(); }

   private:
      std::string oid_name() const override { return "X509v3.CRLDistributionPoints"; }

      bool should_encode() const override { return !m_distribution_points.empty(); }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      std::vector<Distribution_Point> m_distribution_points;
      std::vector<std::string> m_crl_distribution_urls;
};

/**
* CRL Issuing Distribution Point Extension
* todo enforce restrictions from RFC 5280 5.2.5
*/
class CRL_Issuing_Distribution_Point final : public Certificate_Extension {
   public:
      CRL_Issuing_Distribution_Point() = default;

      explicit CRL_Issuing_Distribution_Point(const CRL_Distribution_Points::Distribution_Point& distribution_point) :
            m_distribution_point(distribution_point) {}

      std::unique_ptr<Certificate_Extension> copy() const override {
         return std::make_unique<CRL_Issuing_Distribution_Point>(m_distribution_point);
      }

      const AlternativeName& get_point() const { return m_distribution_point.point(); }

      static OID static_oid() { return OID({2, 5, 29, 28}); }

      OID oid_of() const override { return static_oid(); }

   private:
      std::string oid_name() const override { return "X509v3.CRLIssuingDistributionPoint"; }

      bool should_encode() const override { return true; }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      CRL_Distribution_Points::Distribution_Point m_distribution_point;
};

/**
* OCSP NoCheck Extension
*
* RFC6960 4.2.2.2.1
*    A CA may specify that an OCSP client can trust a responder for the
*    lifetime of the responder's certificate.  The CA does so by
*    including the extension id-pkix-ocsp-nocheck.
*
* In other words: OCSP responder certificates with this extension do not need
*                 to be validated against some revocation info.
*/
class OCSP_NoCheck final : public Certificate_Extension {
   public:
      OCSP_NoCheck() = default;

      std::unique_ptr<Certificate_Extension> copy() const override { return std::make_unique<OCSP_NoCheck>(); }

      static OID static_oid() { return OID({1, 3, 6, 1, 5, 5, 7, 48, 1, 5}); }

      OID oid_of() const override { return static_oid(); }

   private:
      std::string oid_name() const override { return "PKIX.OCSP.NoCheck"; }

      bool should_encode() const override { return true; }

      std::vector<uint8_t> encode_inner() const override;

      void decode_inner(const std::vector<uint8_t>& in) override;
};

/**
* TNAuthList extension
*
* RFC8226 Secure Telephone Identity Credentials
*   https://www.rfc-editor.org/rfc/rfc8226#section-9
*/
class BOTAN_PUBLIC_API(3, 5) TNAuthList final : public Certificate_Extension {
   public:
      class BOTAN_PUBLIC_API(3, 5) Entry final : public ASN1_Object {
         public:
            /* TNEntry choice values
             * see: https://datatracker.ietf.org/doc/html/rfc8226#section-9 */
            enum Type : uint8_t /* NOLINT(*-use-enum-class) */ {
               ServiceProviderCode = 0,
               TelephoneNumberRange = 1,
               TelephoneNumber = 2
            };

            struct TelephoneNumberRangeData {
                  ASN1_String start;  //TelephoneNumber (IA5String)
                  size_t count{};     //2..MAX
            };

            using RangeContainer = std::vector<TelephoneNumberRangeData>;
            using DataContainer = std::variant<ASN1_String, RangeContainer>;

            void encode_into(DER_Encoder& to) const override;
            void decode_from(class BER_Decoder& from) override;

            Type type() const { return m_type; }

            const std::string& service_provider_code() const;

            const RangeContainer& telephone_number_range() const;

            const std::string& telephone_number() const;

         private:
            Type m_type{};
            DataContainer m_data;
      };

      TNAuthList() = default;

      std::unique_ptr<Certificate_Extension> copy() const override { return std::make_unique<TNAuthList>(*this); }

      static OID static_oid() { return OID({1, 3, 6, 1, 5, 5, 7, 1, 26}); }

      OID oid_of() const override { return static_oid(); }

      const std::vector<Entry>& entries() const { return m_tn_entries; }

   private:
      std::string oid_name() const override { return "PKIX.TNAuthList"; }

      bool should_encode() const override { return true; }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      std::vector<Entry> m_tn_entries;
};

/**
 * IP Address Blocks Extension
 *
 * RFC 3779 X.509 Extensions for IP Addr
 *
*/
class BOTAN_PUBLIC_API(3, 9) IPAddressBlocks final : public Certificate_Extension {
   public:
      enum class Version : uint8_t {
         IPv4 = 4,
         IPv6 = 16,
      };

      template <Version V>
      class BOTAN_PUBLIC_API(3, 9) IPAddress final {
            static constexpr size_t Length = static_cast<size_t>(V);

         public:
            explicit IPAddress(std::span<const uint8_t> v);

            std::array<uint8_t, Length> value() const { return m_value; }

         private:
            friend class IPAddressBlocks;
            IPAddress() = default;

            void next() {
               for(auto it = m_value.rbegin(); it != m_value.rend(); it++) {
                  // we increment the current octet
                  (*it)++;
                  // if it did not wrap around we are done, else look at the next octet
                  if(*it != 0) {
                     break;
                  }
               }
            }

            friend IPAddress<V> operator+(IPAddress<V> lhs, size_t rhs) {
               // we only really need to be able to compute +1, so this is fine
               for(size_t i = 0; i < rhs; i++) {
                  lhs.next();
               }
               return IPAddress<V>(lhs);
            }

            friend std::strong_ordering operator<=>(const IPAddress<V> lhs, const IPAddress<V>& rhs) {
               for(size_t i = 0; i < Length; i++) {
                  if(lhs.value()[i] < rhs.value()[i]) {
                     return std::strong_ordering::less;
                  } else if(lhs.value()[i] > rhs.value()[i]) {
                     return std::strong_ordering::greater;
                  }
               }
               return std::strong_ordering::equal;
            }

            friend bool operator==(const IPAddress<V>& lhs, const IPAddress<V>& rhs) {
               return lhs.value() == rhs.value();
            }

            std::array<uint8_t, Length> m_value;
      };

      template <Version V>
      class BOTAN_PUBLIC_API(3, 9) IPAddressOrRange final : public ASN1_Object {
         public:
            void encode_into(DER_Encoder& to) const override;
            void decode_from(BER_Decoder& from) override;

            IPAddressOrRange() = default;

            explicit IPAddressOrRange(const IPAddress<V>& addr) : m_min(addr), m_max(addr) {}

            IPAddressOrRange(const IPAddress<V>& min, const IPAddress<V>& max) : m_min(min), m_max(max) {
               if(max < min) {
                  throw Decoding_Error("IP address ranges must be sorted");
               }
            }

            IPAddress<V> min() const { return m_min; }

            IPAddress<V> max() const { return m_max; }

         private:
            IPAddress<V> m_min{};
            IPAddress<V> m_max{};

            IPAddress<V> decode_single_address(std::vector<uint8_t> decoded, bool min);
      };

      template <Version V>
      class BOTAN_PUBLIC_API(3, 9) IPAddressChoice final : public ASN1_Object {
         public:
            void encode_into(DER_Encoder& to) const override;
            void decode_from(BER_Decoder& from) override;

            const std::optional<std::vector<IPAddressOrRange<V>>>& ranges() const { return m_ip_addr_ranges; }

            IPAddressChoice() = default;

            explicit IPAddressChoice(std::optional<std::span<const IPAddressOrRange<V>>> ranges);

         private:
            std::optional<std::vector<IPAddressOrRange<V>>> m_ip_addr_ranges;
      };

      class BOTAN_PUBLIC_API(3, 9) IPAddressFamily final : public ASN1_Object {
         public:
            typedef std::variant<IPAddressChoice<Version::IPv4>, IPAddressChoice<Version::IPv6>> AddrChoice;

            void encode_into(DER_Encoder& to) const override;
            void decode_from(BER_Decoder& from) override;

            IPAddressFamily() = default;

            explicit IPAddressFamily(const AddrChoice& choice, std::optional<uint8_t> safi = std::nullopt) :
                  m_safi(safi), m_ip_addr_choice(choice) {
               if(std::holds_alternative<IPAddressChoice<Version::IPv4>>(choice)) {
                  m_afi = 1;
               } else {
                  m_afi = 2;
               }
            }

            uint16_t afi() const { return m_afi; }

            std::optional<uint8_t> safi() const { return m_safi; }

            const AddrChoice& addr_choice() const { return m_ip_addr_choice; }

         private:
            uint16_t m_afi = 1;
            std::optional<uint8_t> m_safi;
            AddrChoice m_ip_addr_choice;
      };

      IPAddressBlocks() = default;

      explicit IPAddressBlocks(const std::vector<IPAddressFamily>& blocks) : m_ip_addr_blocks(blocks) {
         this->sort_and_merge();
      }

      std::unique_ptr<Certificate_Extension> copy() const override { return std::make_unique<IPAddressBlocks>(*this); }

      static OID static_oid() { return OID({1, 3, 6, 1, 5, 5, 7, 1, 7}); }

      OID oid_of() const override { return static_oid(); }

      void validate(const X509_Certificate& subject,
                    const std::optional<X509_Certificate>& issuer,
                    const std::vector<X509_Certificate>& cert_path,
                    std::vector<std::set<Certificate_Status_Code>>& cert_status,
                    size_t pos) override;

      /// Add a single IP address to this extension (for the specified SAFI, if any)
      template <Version V>
      void add_address(const std::array<uint8_t, static_cast<size_t>(V)>& address,
                       std::optional<uint8_t> safi = std::nullopt) {
         add_address<V>(address, address, safi);
      }

      /// Add an IP address range to this extension (for the specified SAFI, if any)
      template <Version V>
      void add_address(const std::array<uint8_t, static_cast<std::size_t>(V)>& min,
                       const std::array<uint8_t, static_cast<std::size_t>(V)>& max,
                       std::optional<uint8_t> safi = std::nullopt) {
         std::vector<IPAddressOrRange<V>> addresses = {IPAddressOrRange<V>(IPAddress<V>(min), IPAddress<V>(max))};
         m_ip_addr_blocks.push_back(IPAddressFamily(IPAddressChoice<V>(addresses), safi));
         sort_and_merge();
      }

      /// Make the extension contain no allowed IP addresses for the specified IP version (and SAFI, if any)
      template <Version V>
      void restrict(std::optional<uint8_t> safi = std::nullopt) {
         std::vector<IPAddressOrRange<V>> addresses = {};
         m_ip_addr_blocks.push_back(IPAddressFamily(IPAddressChoice<V>(addresses), safi));
         sort_and_merge();
      }

      /// Mark the specified IP version as 'inherit' (for the specified SAFI, if any)
      template <Version V>
      void inherit(std::optional<uint8_t> safi = std::nullopt) {
         m_ip_addr_blocks.push_back(IPAddressFamily(IPAddressChoice<V>(), safi));
         sort_and_merge();
      }

      const std::vector<IPAddressFamily>& addr_blocks() const { return m_ip_addr_blocks; }

   private:
      std::string oid_name() const override { return "PKIX.IpAddrBlocks"; }

      bool should_encode() const override { return true; }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      std::vector<IPAddressFamily> m_ip_addr_blocks;

      void sort_and_merge();
      template <Version V>
      IPAddressFamily merge(std::vector<IPAddressFamily>& blocks);
};

/**
 * AS Blocks Extension
 *
 * RFC 3779 X.509 Extensions for AS ID
 *
*/
class BOTAN_PUBLIC_API(3, 9) ASBlocks final : public Certificate_Extension {
   public:
      typedef uint32_t asnum_t;

      class BOTAN_PUBLIC_API(3, 9) ASIdOrRange final : public ASN1_Object {
         public:
            void encode_into(DER_Encoder& to) const override;
            void decode_from(BER_Decoder& from) override;

            asnum_t min() const { return m_min; }

            asnum_t max() const { return m_max; }

            ASIdOrRange() = default;

            explicit ASIdOrRange(asnum_t id) : m_min(id), m_max(id) {}

            ASIdOrRange(asnum_t min, asnum_t max) : m_min(min), m_max(max) {
               if(max < min) {
                  throw Decoding_Error("AS range numbers must be sorted");
               }
            }

         private:
            asnum_t m_min = 0;
            asnum_t m_max = 0;
      };

      class BOTAN_PUBLIC_API(3, 9) ASIdentifierChoice final : public ASN1_Object {
         public:
            void encode_into(DER_Encoder& to) const override;
            void decode_from(BER_Decoder& from) override;

            ASIdentifierChoice() = default;

            explicit ASIdentifierChoice(const std::optional<std::vector<ASIdOrRange>>& ranges);

            const std::optional<std::vector<ASIdOrRange>>& ranges() const { return m_as_ranges; }

         private:
            std::optional<std::vector<ASIdOrRange>> m_as_ranges;
      };

      class BOTAN_PUBLIC_API(3, 9) ASIdentifiers final : public ASN1_Object {
         public:
            void encode_into(DER_Encoder& to) const override;
            void decode_from(BER_Decoder& from) override;

            explicit ASIdentifiers(const std::optional<ASIdentifierChoice>& asnum,
                                   const std::optional<ASIdentifierChoice>& rdi) :
                  m_asnum(asnum), m_rdi(rdi) {
               if(!m_asnum.has_value() && !m_rdi.has_value()) {
                  throw Decoding_Error("One of asnum, rdi must be present");
               }
            }

            const std::optional<ASIdentifierChoice>& asnum() const { return m_asnum; }

            const std::optional<ASIdentifierChoice>& rdi() const { return m_rdi; }

         private:
            friend class ASBlocks;
            ASIdentifiers() = default;

            std::optional<ASIdentifierChoice> m_asnum;
            std::optional<ASIdentifierChoice> m_rdi;
      };

      ASBlocks() = default;

      explicit ASBlocks(const ASIdentifiers& as_idents) : m_as_identifiers(as_idents) {}

      std::unique_ptr<Certificate_Extension> copy() const override { return std::make_unique<ASBlocks>(*this); }

      static OID static_oid() { return OID({1, 3, 6, 1, 5, 5, 7, 1, 8}); }

      OID oid_of() const override { return static_oid(); }

      void validate(const X509_Certificate& subject,
                    const std::optional<X509_Certificate>& issuer,
                    const std::vector<X509_Certificate>& cert_path,
                    std::vector<std::set<Certificate_Status_Code>>& cert_status,
                    size_t pos) override;

      /// Add a single asnum to this extension
      void add_asnum(asnum_t asnum) { add_asnum(asnum, asnum); }

      /// Add an asnum range to this extension
      void add_asnum(asnum_t min, asnum_t max) {
         m_as_identifiers = ASIdentifiers(add_new(m_as_identifiers.asnum(), min, max), m_as_identifiers.rdi());
      }

      /// Make the extension contain no allowed asnum's
      void restrict_asnum() {
         std::vector<ASIdOrRange> empty;
         m_as_identifiers = ASIdentifiers(ASIdentifierChoice(empty), m_as_identifiers.rdi());
      }

      /// Mark the asnum entry as 'inherit'
      void inherit_asnum() { m_as_identifiers = ASIdentifiers(ASIdentifierChoice(), m_as_identifiers.rdi()); }

      /// Add a single rdi to this extension
      void add_rdi(asnum_t rdi) { add_rdi(rdi, rdi); }

      /// Add an rdi range to this extension
      void add_rdi(asnum_t min, asnum_t max) {
         m_as_identifiers = ASIdentifiers(m_as_identifiers.asnum(), add_new(m_as_identifiers.rdi(), min, max));
      }

      /// Make the extension contain no allowed rdi's
      void restrict_rdi() {
         std::vector<ASIdOrRange> empty;
         m_as_identifiers = ASIdentifiers(m_as_identifiers.asnum(), ASIdentifierChoice(empty));
      }

      /// Mark the rdi entry as 'inherit'
      void inherit_rdi() { m_as_identifiers = ASIdentifiers(m_as_identifiers.asnum(), ASIdentifierChoice()); }

      const ASIdentifiers& as_identifiers() const { return m_as_identifiers; }

   private:
      ASIdentifiers m_as_identifiers;

      std::string oid_name() const override { return "PKIX.AutonomousSysIds"; }

      bool should_encode() const override { return true; }

      static ASIdentifierChoice add_new(const std::optional<ASIdentifierChoice>& old, asnum_t min, asnum_t max);

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;
};

/**
* An unknown X.509 extension
* Will add a failure to the path validation result, if critical
*/
class BOTAN_PUBLIC_API(2, 4) Unknown_Extension final : public Certificate_Extension {
   public:
      Unknown_Extension(const OID& oid, bool critical, bool failed_to_decode = false) :
            m_oid(oid), m_critical(critical), m_failed_to_decode(failed_to_decode) {}

      std::unique_ptr<Certificate_Extension> copy() const override {
         return std::make_unique<Unknown_Extension>(m_oid, m_critical, m_failed_to_decode);
      }

      /**
      * Return the OID of this unknown extension
      */
      OID oid_of() const override { return m_oid; }

      //static_oid not defined for Unknown_Extension

      /**
      * Return the extension contents
      */
      const std::vector<uint8_t>& extension_contents() const { return m_bytes; }

      /**
      * Return if this extension was marked critical
      */
      bool is_critical_extension() const { return m_critical; }

      /**
      * Return true if this extension's OID was recognized but the contents
      * failed to decode.
      */
      bool failed_to_decode() const { return m_failed_to_decode; }

      void validate(const X509_Certificate& /*subject*/,
                    const std::optional<X509_Certificate>& /*issuer*/,
                    const std::vector<X509_Certificate>& /*cert_path*/,
                    std::vector<std::set<Certificate_Status_Code>>& cert_status,
                    size_t pos) override {
         if(m_failed_to_decode) {
            cert_status.at(pos).insert(Certificate_Status_Code::EXTENSION_ENCODING_ERROR);
         } else if(m_critical) {
            cert_status.at(pos).insert(Certificate_Status_Code::UNKNOWN_CRITICAL_EXTENSION);
         }
      }

   private:
      std::string oid_name() const override { return ""; }

      bool should_encode() const override { return true; }

      std::vector<uint8_t> encode_inner() const override;
      void decode_inner(const std::vector<uint8_t>& in) override;

      OID m_oid;
      bool m_critical;
      bool m_failed_to_decode;
      std::vector<uint8_t> m_bytes;
};

}  // namespace Cert_Extension

}  // namespace Botan

namespace Botan::X509 {

/**
* BER encode a key
* @param key the public key to encode
* @return BER encoding of this key
*/
inline std::vector<uint8_t> BER_encode(const Public_Key& key) {
   return key.subject_public_key();
}

/**
* PEM encode a public key into a string.
* @param key the key to encode
* @return PEM encoded key
*/
BOTAN_PUBLIC_API(2, 0) std::string PEM_encode(const Public_Key& key);

/**
* Create a public key from a data source.
* @param source the source providing the DER or PEM encoded key
* @return new public key object
*/
BOTAN_PUBLIC_API(3, 0) std::unique_ptr<Public_Key> load_key(DataSource& source);

#if defined(BOTAN_TARGET_OS_HAS_FILESYSTEM)
/**
* Create a public key from a file
* @param filename pathname to the file to load
* @return new public key object
*/
inline std::unique_ptr<Public_Key> load_key(std::string_view filename) {
   DataSource_Stream source(filename, true);
   return X509::load_key(source);
}
#endif

/**
* Create a public key from a memory region.
* @param enc the memory region containing the DER or PEM encoded key
* @return new public key object
*/
inline std::unique_ptr<Public_Key> load_key(const std::vector<uint8_t>& enc) {
   DataSource_Memory source(enc);
   return X509::load_key(source);
}

/**
* Create a public key from a memory region.
* @param enc the memory region containing the DER or PEM encoded key
* @return new public key object
*/
inline std::unique_ptr<Public_Key> load_key(std::span<const uint8_t> enc) {
   DataSource_Memory source(enc);
   return X509::load_key(source);
}

/**
* Copy a key.
* @param key the public key to copy
* @return new public key object
*/
inline std::unique_ptr<Public_Key> copy_key(const Public_Key& key) {
   DataSource_Memory source(PEM_encode(key));
   return X509::load_key(source);
}

}  // namespace Botan::X509

#if defined(BOTAN_TARGET_OS_HAS_THREADS) && defined(BOTAN_HAS_HTTP_UTIL)
   #define BOTAN_HAS_ONLINE_REVOCATION_CHECKS
#endif

namespace Botan {

/**
* This type represents the validation status of an entire certificate path.
* There is one set of status codes for each certificate in the path.
*/
typedef std::vector<std::set<Certificate_Status_Code>> CertificatePathStatusCodes;

/**
* Specifies restrictions on the PKIX path validation
*/
class BOTAN_PUBLIC_API(2, 0) Path_Validation_Restrictions final {
   public:
      /**
      * @param require_rev if true, revocation information is required

      * @param minimum_key_strength is the minimum strength (in terms of
      *    operations, eg 80 means 2^80) of a signature. Signatures weaker than
      *    this are rejected. If more than 80, SHA-1 signatures are also
      *    rejected. If possible use at least setting 110.
      *
      *        80 bit strength requires 1024 bit RSA
      *        110 bit strength requires 2k bit RSA
      *        128 bit strength requires ~3k bit RSA or P-256
      * @param ocsp_all_intermediates Make OCSP requests for all CAs as
      * well as end entity (if OCSP enabled in path validation request)
      * @param max_ocsp_age maximum age of OCSP responses w/o next_update.
      *        If zero, there is no maximum age
      * @param trusted_ocsp_responders certificate store containing certificates
      *        of trusted OCSP responders (additionally to the CA's responders)
      * @param ignore_trusted_root_time_range if true, validity checks on the
      *        time range of the trusted root certificate only produce warnings
      * @param require_self_signed_trust_anchors if true, only self-signed certificates
      *        are allowed as trust anchors. Trust anchors based on intermediate
      *        and leaf certificates are forbidden in this case.
      */
      BOTAN_FUTURE_EXPLICIT Path_Validation_Restrictions(
         bool require_rev = false,
         size_t minimum_key_strength = 110,
         bool ocsp_all_intermediates = false,
         std::chrono::seconds max_ocsp_age = std::chrono::seconds::zero(),
         std::unique_ptr<Certificate_Store> trusted_ocsp_responders = std::make_unique<Certificate_Store_In_Memory>(),
         bool ignore_trusted_root_time_range = false,
         bool require_self_signed_trust_anchors = true);

      /**
      * @param require_rev if true, revocation information is required
      * @param minimum_key_strength is the minimum strength (in terms of
      *        operations, eg 80 means 2^80) of a signature. Signatures
      *        weaker than this are rejected.
      * @param ocsp_all_intermediates Make OCSP requests for all CAs as
      * well as end entity (if OCSP enabled in path validation request)
      * @param trusted_hashes a set of trusted hashes. Any signatures
      *        created using a hash other than one of these will be
      *        rejected.
      * @param max_ocsp_age maximum age of OCSP responses w/o next_update.
      *        If zero, there is no maximum age
      * @param trusted_ocsp_responders certificate store containing certificates
      *        of trusted OCSP responders (additionally to the CA's responders)
      * @param ignore_trusted_root_time_range if true, validity checks on the
      *        time range of the trusted root certificate only produce warnings
      * @param require_self_signed_trust_anchors if true, only self-signed certificates
      *        are allowed as trust anchors. Trust anchors based on intermediate
      *        and leaf certificates are forbidden in this case.
      */
      Path_Validation_Restrictions(
         bool require_rev,
         size_t minimum_key_strength,
         bool ocsp_all_intermediates,
         const std::set<std::string>& trusted_hashes,
         std::chrono::seconds max_ocsp_age = std::chrono::seconds::zero(),
         std::unique_ptr<Certificate_Store> trusted_ocsp_responders = std::make_unique<Certificate_Store_In_Memory>(),
         bool ignore_trusted_root_time_range = false,
         bool require_self_signed_trust_anchors = true) :
            m_require_revocation_information(require_rev),
            m_ocsp_all_intermediates(ocsp_all_intermediates),
            m_trusted_hashes(trusted_hashes),
            m_minimum_key_strength(minimum_key_strength),
            m_max_ocsp_age(max_ocsp_age),
            m_trusted_ocsp_responders(std::move(trusted_ocsp_responders)),
            m_ignore_trusted_root_time_range(ignore_trusted_root_time_range),
            m_require_self_signed_trust_anchors(require_self_signed_trust_anchors) {}

      /**
      * @return whether revocation information is required
      */
      bool require_revocation_information() const { return m_require_revocation_information; }

      /**
      * @return whether all intermediate CAs should also be OCSPed. If false
      * then only end entity OCSP is required/requested.
      */
      bool ocsp_all_intermediates() const { return m_ocsp_all_intermediates; }

      /**
      * @return trusted signature hash functions
      */
      const std::set<std::string>& trusted_hashes() const { return m_trusted_hashes; }

      /**
      * @return minimum required key strength
      */
      size_t minimum_key_strength() const { return m_minimum_key_strength; }

      /**
      * @return maximum age of OCSP responses w/o next_update.
      * If zero, there is no maximum age
      */
      std::chrono::seconds max_ocsp_age() const { return m_max_ocsp_age; }

      /**
       * Certificates in this store are trusted to sign OCSP responses
       * additionally to the CA's responder certificates.
       * @return certificate store containing trusted OCSP responder certs
       */
      const Certificate_Store* trusted_ocsp_responders() const { return m_trusted_ocsp_responders.get(); }

      /**
       * RFC 5280 does not disallow trusted anchors signing certificates with wider validity
       * ranges than theirs. When checking a certificate chain at a specific
       * point in time, this can lead to situations where a root certificate is expired, but
       * the lower-chain certificates are not.
       *
       * If this flag is set to true, such chains are considered valid (with warning
       * TRUSTED_CERT_HAS_EXPIRED). Otherwise, the chain is rejected with the error
       * code CERT_HAS_EXPIRED. The same holds for not yet valid certificates with the
       * error code CERT_NOT_YET_VALID (or warning TRUSTED_CERT_NOT_YET_VALID).
       */
      bool ignore_trusted_root_time_range() const { return m_ignore_trusted_root_time_range; }

      /**
       * By default Botan requires trust anchors to be self-signed.
       * This prevents using intermediate CA certificates and leaf certificates
       * as trust anchors, even if they are included in the Certificate Store.
       * This restriction can be removed by setting
       * require_self_signed_trust_anchors=false in the constructor.
       */
      bool require_self_signed_trust_anchors() const { return m_require_self_signed_trust_anchors; }

   private:
      bool m_require_revocation_information;
      bool m_ocsp_all_intermediates;
      std::set<std::string> m_trusted_hashes;
      size_t m_minimum_key_strength;
      std::chrono::seconds m_max_ocsp_age;
      std::unique_ptr<Certificate_Store> m_trusted_ocsp_responders;
      bool m_ignore_trusted_root_time_range;
      bool m_require_self_signed_trust_anchors;
};

/**
* Represents the result of a PKIX path validation
*/
class BOTAN_PUBLIC_API(2, 0) Path_Validation_Result final {
   public:
      typedef Certificate_Status_Code Code;

      /**
      * @return the trust root of the validation if successful
      * throws an exception if the validation failed
      */
      const X509_Certificate& trust_root() const;

      /**
      * @return the full path from subject to trust root
      * This path may be empty
      */
      const std::vector<X509_Certificate>& cert_path() const { return m_cert_path; }

      /**
      * @return true iff the validation was successful
      */
      bool successful_validation() const;

      /**
      * @return true iff no warnings occurred during validation
      */
      bool no_warnings() const;

      /**
      * @return overall validation result code
      */
      Certificate_Status_Code result() const { return m_overall; }

      /**
      * @return a set of status codes for each certificate in the chain
      */
      const CertificatePathStatusCodes& all_statuses() const { return m_all_status; }

      /**
      * @return the subset of status codes that are warnings
      */
      CertificatePathStatusCodes warnings() const;

      /**
      * @return string representation of the validation result
      */
      std::string result_string() const;

      /**
      * @return string representation of the warnings
      */
      std::string warnings_string() const;

      /**
      * @param code validation status code
      * @return corresponding validation status message
      */
      static const char* status_string(Certificate_Status_Code code);

      /**
      * Create a Path_Validation_Result
      * @param status list of validation status codes
      * @param cert_chain the certificate chain that was validated
      */
      Path_Validation_Result(CertificatePathStatusCodes status, std::vector<X509_Certificate>&& cert_chain);

      /**
      * Create a Path_Validation_Result
      * @param status validation status code
      */
      explicit Path_Validation_Result(Certificate_Status_Code status) : m_overall(status) {}

   private:
      CertificatePathStatusCodes m_all_status;
      CertificatePathStatusCodes m_warnings;
      std::vector<X509_Certificate> m_cert_path;
      Certificate_Status_Code m_overall;
};

/**
* PKIX Path Validation
* @param end_certs certificate chain to validate (with end entity certificate in end_certs[0])
* @param restrictions path validation restrictions
* @param trusted_roots list of certificate stores that contain trusted certificates
* @param hostname if not empty, compared against the DNS name in end_certs[0]
* @param usage if not set to UNSPECIFIED, compared against the key usage in end_certs[0]
* @param validation_time what reference time to use for validation
* @param ocsp_timeout timeout for OCSP operations, 0 disables OCSP check
* @param ocsp_resp additional OCSP responses to consider (eg from peer)
* @return result of the path validation
*   note: when enabled, OCSP check is softfail by default: if the OCSP server is not
*   reachable, Path_Validation_Result::successful_validation() will return true.
*   Hardfail OCSP check can be achieve by also calling Path_Validation_Result::no_warnings().
*/
Path_Validation_Result BOTAN_PUBLIC_API(2, 0)
   x509_path_validate(const std::vector<X509_Certificate>& end_certs,
                      const Path_Validation_Restrictions& restrictions,
                      const std::vector<Certificate_Store*>& trusted_roots,
                      std::string_view hostname = "",
                      Usage_Type usage = Usage_Type::UNSPECIFIED,
                      std::chrono::system_clock::time_point validation_time = std::chrono::system_clock::now(),
                      std::chrono::milliseconds ocsp_timeout = std::chrono::milliseconds(0),
                      const std::vector<std::optional<OCSP::Response>>& ocsp_resp = {});

/**
* PKIX Path Validation
* @param end_cert certificate to validate
* @param restrictions path validation restrictions
* @param trusted_roots list of stores that contain trusted certificates
* @param hostname if not empty, compared against the DNS name in end_cert
* @param usage if not set to UNSPECIFIED, compared against the key usage in end_cert
* @param validation_time what reference time to use for validation
* @param ocsp_timeout timeout for OCSP operations, 0 disables OCSP check
* @param ocsp_resp additional OCSP responses to consider (eg from peer)
* @return result of the path validation
*/
Path_Validation_Result BOTAN_PUBLIC_API(2, 0)
   x509_path_validate(const X509_Certificate& end_cert,
                      const Path_Validation_Restrictions& restrictions,
                      const std::vector<Certificate_Store*>& trusted_roots,
                      std::string_view hostname = "",
                      Usage_Type usage = Usage_Type::UNSPECIFIED,
                      std::chrono::system_clock::time_point validation_time = std::chrono::system_clock::now(),
                      std::chrono::milliseconds ocsp_timeout = std::chrono::milliseconds(0),
                      const std::vector<std::optional<OCSP::Response>>& ocsp_resp = {});

/**
* PKIX Path Validation
* @param end_cert certificate to validate
* @param restrictions path validation restrictions
* @param store store that contains trusted certificates
* @param hostname if not empty, compared against the DNS name in end_cert
* @param usage if not set to UNSPECIFIED, compared against the key usage in end_cert
* @param validation_time what reference time to use for validation
* @param ocsp_timeout timeout for OCSP operations, 0 disables OCSP check
* @param ocsp_resp additional OCSP responses to consider (eg from peer)
* @return result of the path validation
*/
Path_Validation_Result BOTAN_PUBLIC_API(2, 0)
   x509_path_validate(const X509_Certificate& end_cert,
                      const Path_Validation_Restrictions& restrictions,
                      const Certificate_Store& store,
                      std::string_view hostname = "",
                      Usage_Type usage = Usage_Type::UNSPECIFIED,
                      std::chrono::system_clock::time_point validation_time = std::chrono::system_clock::now(),
                      std::chrono::milliseconds ocsp_timeout = std::chrono::milliseconds(0),
                      const std::vector<std::optional<OCSP::Response>>& ocsp_resp = {});

/**
* PKIX Path Validation
* @param end_certs certificate chain to validate
* @param restrictions path validation restrictions
* @param store store that contains trusted certificates
* @param hostname if not empty, compared against the DNS name in end_certs[0]
* @param usage if not set to UNSPECIFIED, compared against the key usage in end_certs[0]
* @param validation_time what reference time to use for validation
* @param ocsp_timeout timeout for OCSP operations, 0 disables OCSP check
* @param ocsp_resp additional OCSP responses to consider (eg from peer)
* @return result of the path validation
*/
Path_Validation_Result BOTAN_PUBLIC_API(2, 0)
   x509_path_validate(const std::vector<X509_Certificate>& end_certs,
                      const Path_Validation_Restrictions& restrictions,
                      const Certificate_Store& store,
                      std::string_view hostname = "",
                      Usage_Type usage = Usage_Type::UNSPECIFIED,
                      std::chrono::system_clock::time_point validation_time = std::chrono::system_clock::now(),
                      std::chrono::milliseconds ocsp_timeout = std::chrono::milliseconds(0),
                      const std::vector<std::optional<OCSP::Response>>& ocsp_resp = {});

/**
* namespace PKIX holds the building blocks that are called by x509_path_validate.
* This allows custom validation logic to be written by applications and makes
* for easier testing, but unless you're positive you know what you're doing you
* probably want to just call x509_path_validate instead.
*/
namespace PKIX {

/**
* Create all certificate paths by identifying all possible routes from the
* end-entity certificate to any certificate in the certificate store list. Paths
* may also end in intermediate or leaf certificates found in the certificate
* stores.
*
* WARNING: The validity (e.g. signatures or constraints) of the output path IS
* NOT checked.
*
* @param cert_paths output parameter to be filled with all discovered certificate paths
* @param trusted_certstores list of certificate stores that contain trusted certificates
* @param end_entity the cert to be validated
* @param end_entity_extra optional list of additional untrusted certs for path building
* @return result of the path building operation (OK or error)
*/
Certificate_Status_Code BOTAN_PUBLIC_API(3, 11)
   build_all_certificate_paths(std::vector<std::vector<X509_Certificate>>& cert_paths,
                               const std::vector<Certificate_Store*>& trusted_certstores,
                               const X509_Certificate& end_entity,
                               const std::vector<X509_Certificate>& end_entity_extra);

/**
* Same as build_all_certificate_paths but only outputs a single path. If there are
* paths ending in self-signed certificates, these are prioritized over paths ending
* in intermediate or leaf certificates of the certificate store.
*
* WARNING: The validity (e.g. signatures or constraints) of the output path IS
* NOT checked.
*
* @param cert_path_out output parameter, cert_path will be appended to this vector
* @param trusted_certstores list of certificate stores that contain trusted certificates
* @param end_entity the cert to be validated
* @param end_entity_extra optional list of additional untrusted certs for path building
* @return result of the path building operation (OK or error)
*/
BOTAN_DEPRECATED("Use build_all_certificate_paths")
Certificate_Status_Code BOTAN_PUBLIC_API(2, 0)
   build_certificate_path(std::vector<X509_Certificate>& cert_path_out,
                          const std::vector<Certificate_Store*>& trusted_certstores,
                          const X509_Certificate& end_entity,
                          const std::vector<X509_Certificate>& end_entity_extra);

/**
* Check the certificate chain, but not any revocation data
*
* @param cert_path path built by build_certificate_path with OK result.
* The first element is the end entity certificate, the last element is
* the trusted root certificate.
* @param ref_time whatever time you want to perform the validation
* against (normally current system clock)
* @param hostname the hostname
* @param usage end entity usage checks
* @param restrictions the relevant path validation restrictions object
* @return vector of results on per certificate in the path, each containing a set of
* results. If all codes in the set are < Certificate_Status_Code::FIRST_ERROR_STATUS,
* then the result for that certificate is successful. If all results are
*/
CertificatePathStatusCodes BOTAN_PUBLIC_API(3, 0) check_chain(const std::vector<X509_Certificate>& cert_path,
                                                              std::chrono::system_clock::time_point ref_time,
                                                              std::string_view hostname,
                                                              Usage_Type usage,
                                                              const Path_Validation_Restrictions& restrictions);

/**
* Check OCSP responses for revocation information
* @param cert_path path already validated by check_chain
* @param ocsp_responses the OCSP responses to consider
* @param certstores trusted roots
* @param ref_time whatever time you want to perform the validation against
* (normally current system clock)
* @param restrictions the relevant path validation restrictions object
* @return revocation status
*/
CertificatePathStatusCodes BOTAN_PUBLIC_API(3, 0)
   check_ocsp(const std::vector<X509_Certificate>& cert_path,
              const std::vector<std::optional<OCSP::Response>>& ocsp_responses,
              const std::vector<Certificate_Store*>& certstores,
              std::chrono::system_clock::time_point ref_time,
              const Path_Validation_Restrictions& restrictions);

/**
* Check CRLs for revocation information
* @param cert_path path already validated by check_chain
* @param crls the list of CRLs to check, it is assumed that crls[i] (if not null)
* is the associated CRL for the subject in cert_path[i].
* @param ref_time whatever time you want to perform the validation against
* (normally current system clock)
* @return revocation status
*/
CertificatePathStatusCodes BOTAN_PUBLIC_API(2, 0) check_crl(const std::vector<X509_Certificate>& cert_path,
                                                            const std::vector<std::optional<X509_CRL>>& crls,
                                                            std::chrono::system_clock::time_point ref_time);

/**
* Check CRLs for revocation information
* @param cert_path path already validated by check_chain
* @param certstores a list of certificate stores to query for the CRL
* @param ref_time whatever time you want to perform the validation against
* (normally current system clock)
* @return revocation status
*/
CertificatePathStatusCodes BOTAN_PUBLIC_API(2, 0) check_crl(const std::vector<X509_Certificate>& cert_path,
                                                            const std::vector<Certificate_Store*>& certstores,
                                                            std::chrono::system_clock::time_point ref_time);

#if defined(BOTAN_HAS_ONLINE_REVOCATION_CHECKS)

/**
* Check OCSP using online (HTTP) access. Current version creates a thread and
* network connection per OCSP request made.
*
* @param cert_path path already validated by check_chain
* @param trusted_certstores a list of certstores with trusted certs
* @param ref_time whatever time you want to perform the validation against
* (normally current system clock)
* @param timeout for timing out the responses, though actually this function
* may block for up to timeout*cert_path.size()*C for some small C.
* @param restrictions the relevant path validation restrictions object
* @return revocation status
*/
CertificatePathStatusCodes BOTAN_PUBLIC_API(3, 0)
   check_ocsp_online(const std::vector<X509_Certificate>& cert_path,
                     const std::vector<Certificate_Store*>& trusted_certstores,
                     std::chrono::system_clock::time_point ref_time,
                     std::chrono::milliseconds timeout,
                     const Path_Validation_Restrictions& restrictions);

/**
* Check CRL using online (HTTP) access. Current version creates a thread and
* network connection per CRL access.

* @param cert_path path already validated by check_chain
* @param trusted_certstores a list of certstores with trusted certs
* @param certstore_to_recv_crls optional (nullptr to disable), all CRLs
* retrieved will be saved to this cert store.
* @param ref_time whatever time you want to perform the validation against
* (normally current system clock)
* @param timeout for timing out the responses, though actually this function
* may block for up to timeout*cert_path.size()*C for some small C.
* @return revocation status
*/
CertificatePathStatusCodes BOTAN_PUBLIC_API(2, 0)
   check_crl_online(const std::vector<X509_Certificate>& cert_path,
                    const std::vector<Certificate_Store*>& trusted_certstores,
                    Certificate_Store_In_Memory* certstore_to_recv_crls,
                    std::chrono::system_clock::time_point ref_time,
                    std::chrono::milliseconds timeout);

#endif

/**
* Find overall status (OK, error) of a validation
* @param cert_status result of merge_revocation_status or check_chain
*/
Certificate_Status_Code BOTAN_PUBLIC_API(2, 0) overall_status(const CertificatePathStatusCodes& cert_status);

/**
* Merge the results from CRL and/or OCSP checks into chain_status
* @param chain_status the certificate status
* @param crl_status results from check_crl
* @param ocsp_status results from check_ocsp
* @param restrictions the relevant path validation restrictions object
*/
void BOTAN_PUBLIC_API(3, 0) merge_revocation_status(CertificatePathStatusCodes& chain_status,
                                                    const CertificatePathStatusCodes& crl_status,
                                                    const CertificatePathStatusCodes& ocsp_status,
                                                    const Path_Validation_Restrictions& restrictions);

}  // namespace PKIX

}  // namespace Botan

namespace Botan {

class RandomNumberGenerator;
class Private_Key;

/**
* Options for X.509 certificates.
*/
class BOTAN_PUBLIC_API(2, 0) X509_Cert_Options final {
   public:
      /**
      * the subject common name
      */
      std::string common_name;  // NOLINT(*non-private-member-variable*)

      /**
      * the subject country
      */
      std::string country;  // NOLINT(*non-private-member-variable*)

      /**
      * the subject organization
      */
      std::string organization;  // NOLINT(*non-private-member-variable*)

      /**
      * the subject organizational unit
      */
      std::string org_unit;  // NOLINT(*non-private-member-variable*)

      /**
       * additional subject organizational units.
       */
      std::vector<std::string> more_org_units;  // NOLINT(*non-private-member-variable*)

      /**
      * the subject locality
      */
      std::string locality;  // NOLINT(*non-private-member-variable*)

      /**
      * the subject state
      */
      std::string state;  // NOLINT(*non-private-member-variable*)

      /**
      * the subject serial number
      */
      std::string serial_number;  // NOLINT(*non-private-member-variable*)

      /**
      * the subject email address
      */
      std::string email;  // NOLINT(*non-private-member-variable*)

      /**
      * the subject URI
      */
      std::string uri;  // NOLINT(*non-private-member-variable*)

      /**
      * the subject IPv4 address
      */
      std::string ip;  // NOLINT(*non-private-member-variable*)

      /**
      * the subject DNS
      */
      std::string dns;  // NOLINT(*non-private-member-variable*)

      /**
       * additional subject DNS entries.
       */
      std::vector<std::string> more_dns;  // NOLINT(*non-private-member-variable*)

      /**
      * the subject XMPP
      */
      std::string xmpp;  // NOLINT(*non-private-member-variable*)

      /**
      * the subject challenge password
      */
      std::string challenge;  // NOLINT(*non-private-member-variable*)

      /**
      * the subject notBefore
      */
      X509_Time start;  // NOLINT(*non-private-member-variable*)
      /**
      * the subject notAfter
      */
      X509_Time end;  // NOLINT(*non-private-member-variable*)

      /**
      * Indicates whether the certificate request
      */
      bool is_CA = false;  // NOLINT(*non-private-member-variable*)

      /**
      * Indicates the BasicConstraints path limit
      */
      size_t path_limit = 0;  // NOLINT(*non-private-member-variable*)

      /**
      * Padding scheme to use. If empty uses a default
      */
      std::string padding_scheme;  // NOLINT(*non-private-member-variable*)

      /**
      * The key constraints for the subject public key
      */
      Key_Constraints constraints;  // NOLINT(*non-private-member-variable*)

      /**
      * The key extended constraints for the subject public key
      */
      std::vector<OID> ex_constraints;  // NOLINT(*non-private-member-variable*)

      /**
      * Additional X.509 extensions
      */
      Extensions extensions;  // NOLINT(*non-private-member-variable*)

      /**
      * Mark the certificate as a CA certificate and set the path limit.
      * @param limit the path limit to be set in the BasicConstraints extension.
      */
      void CA_key(size_t limit = 1);

      /**
      * Choose a padding scheme different from the default for the key used.
      */
      void set_padding_scheme(std::string_view scheme);

      /**
      * Set the notBefore of the certificate.
      * @param time the notBefore value of the certificate
      */
      void not_before(std::string_view time);

      /**
      * Set the notAfter of the certificate.
      * @param time the notAfter value of the certificate
      */
      void not_after(std::string_view time);

      /**
      * Add the key constraints of the KeyUsage extension.
      * @param constr the constraints to set
      */
      void add_constraints(Key_Constraints constr);

      /**
      * Add constraints to the ExtendedKeyUsage extension.
      * @param oid the oid to add
      */
      void add_ex_constraint(const OID& oid);

      /**
      * Add constraints to the ExtendedKeyUsage extension.
      * @param name the name to look up the oid to add
      */
      void add_ex_constraint(std::string_view name);

      /**
      * Construct a new options object
      * @param opts define the common name of this object. An example for this
      * parameter would be "common_name/country/organization/organizational_unit".
      * @param expire_time the expiration time (from the current clock in seconds)
      */
      BOTAN_FUTURE_EXPLICIT X509_Cert_Options(std::string_view opts = "", uint32_t expire_time = 365 * 24 * 60 * 60);
};

namespace X509 {

/**
* Create a self-signed X.509 certificate.
* @param opts the options defining the certificate to create
* @param key the private key used for signing, i.e. the key
* associated with this self-signed certificate
* @param hash_fn the hash function to use
* @param rng the rng to use
* @return newly created self-signed certificate
*/
BOTAN_PUBLIC_API(2, 0)
X509_Certificate create_self_signed_cert(const X509_Cert_Options& opts,
                                         const Private_Key& key,
                                         std::string_view hash_fn,
                                         RandomNumberGenerator& rng);

/**
* Create a PKCS#10 certificate request.
* @param opts the options defining the request to create
* @param key the key used to sign this request
* @param rng the rng to use
* @param hash_fn the hash function to use
* @return newly created PKCS#10 request
*/
BOTAN_PUBLIC_API(2, 0)
PKCS10_Request create_cert_req(const X509_Cert_Options& opts,
                               const Private_Key& key,
                               std::string_view hash_fn,
                               RandomNumberGenerator& rng);

}  // namespace X509

}  // namespace Botan

#endif // BOTAN_AMALGAMATION_H_
