/**
 * @file encodex.h
 * @brief Provides support for various data encoding schemes common to cryptography.
 * @author Anthony @e ACagliano Cagliano
 *
 * @mainpage ENCODEX
 * A library providing encoding support for various data formats
 * 1. ASN.1 parser
 * 2. Base64 encoding/decoding
 * tbc
 */

#ifndef ENCODEX_H
#define ENCODEX_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


// ##########################
// ###### ASN.1 Parser ######
// ##########################
/*
 * ASN stands for Abstract Syntax Notation.
 * It is a standard notation language for defining data structures.
 * It is commonly used for the encoding of key data by various cryptography libraries.
 * Ex: DER-formatted keys use a modified version of ASN.1.
 */


/*************************************************
 * @typedef asn1\_obj\_t
 * Defines a struct type for extracting ASN.1 element metadata
 * See @b asn1_decode.
 */
typedef struct _asn1_obj_t {
	uint8_t tag;			/**< Defines the ASN.1 element basic tag (low 5 bits of the id). See @b ASN1_TYPES. */
	uint8_t f_class;		/**< Defines the ASN.1 class (high 2 bits of the id). See @b ASN1_CLASSES. */
	bool f_form;			/**< Defines the ASN.1 construction scheme (bit 5 of the id). See @b ASN1_FORMS. */
	size_t len;				/**< Defines the length of the data portion of the element */
	uint8_t *data;			/**< Defines a pointer to the data portion of the element */
} asn1_obj_t;

/*********************************
 * @enum ASN1\_TYPES
 * Defines tag identifiers for ASN.1 encoding
 * See @b asn1_obj_t.tag.
 */
enum ASN1_TYPES {
	ASN1_RESVD = 0,
	ASN1_BOOLEAN,
	ASN1_INTEGER,
	ASN1_BITSTRING,
	ASN1_OCTETSTRING,
	ASN1_NULL,
	ASN1_OBJECTID,
	ASN1_OBJECTDESC,
	ASN1_INSTANCE,
	ASN1_REAL,
	ASN1_ENUMERATED,
	ASN1_EMBEDDEDPDV,
	ASN1_UTF8STRING,
	ASN1_RELATIVEOID,
	ASN1_SEQUENCE = 16,
	ASN1_SET,
	ASN1_NUMERICSTRING,
	ASN1_PRINTABLESTRING,
	ASN1_TELETEXSTRING,
	ASN1_VIDEOTEXSTRING,
	ASN1_IA5STRING,
	ASN1_UTCTIME,
	ASN1_GENERALIZEDTIME,
	ASN1_GRAPHICSTRING,
	ASN1_VISIBLESTRING,
	ASN1_GENERALSTRING,
	ASN1_UNIVERSALSTRING,
	ASN1_CHARSTRING,
	ASN1_BMPSTRING
};

/**********************************
 * @enum ASN1\_CLASSES
 * Defines class identifiers for ASN.1 encoding.
 * See @b asn1_obj_t.f_class.
 */
enum ASN1_CLASSES {
	ASN1_UNIVERSAL,			/**< tags defined in the ASN.1 standard. Most use cases on calc will be this. */
	ASN1_APPLICATION,		/**< tags unique to a particular application. */
	ASN1_CONTEXTSPEC,		/**< tags that need to be identified within a particular, well-definded context. */
	ASN1_PRIVATE			/**< reserved for use by a specific entity for their applications. */
};

/********************
 * @enum ASN1\_FORMS
 * Defines form identifiers for ASN.1 encoding.
 * See @b asn1_obj_t.f_form.
 */
enum ASN1_FORMS {
	ASN1_PRIMITIVE,			/**< data type that cannot be broken down further. */
	ASN1_CONSTRUCTED		/**< data type composed of multiple primitive data types. */
};

/****************************************************************
 * @brief Parses ASN.1 encoded data and returns metadata into an array of structs.
 * @note This function is recursive for any element of @b constructed form.
 * @note For DER-formatted RSA public keys, you will need to call this function twice to
 * unpack the modulus and exponent. The second time should be on the
 * @b ASN1_BITSTRING that encodes the modulus
 * and public exponent. See the asn1\_decode demo for details.
 * @param asn1_data Pointer to ASN.1-encoded data.
 * @param len The length of the encoded data.
 * @param objs Pointer to an array of @b asn1_obj_t structs to fill with decoded data.
 * @param iter_count Maximum number of ASN.1 elements to process before returning.
 * @returns The number of objects returned by the parser. Zero indicates an error.
 */
size_t asn1_decode(void *asn1_data, size_t len, asn1_obj_t *objs, size_t iter_count);



// ###########################
// ###### Base64 Parser ######
// ###########################
/*
 * Base64 encodes data as a sextet (where each byte corresponds
 * to 6 bits of the input stream) which is then mapped to one of
 * 64 printable characters, or the = padding character.
 * Base64 is often used to encode cryptographic data such as
 * the PEM key format, bcrypt, and more.
 */

/***************************************************************
 * @brief Converts an octet-encoded byte stream into a sextet-encoded byte stream.
 * @param in Pointer to octet-encoded data stream.
 * @param len Length of octet-encoded data stream.
 * @param out Pointer to sextet-encoded data stream.
 * @note @b out should be at least  len * 4 / 3 bytes large.
 * @returns Length of output sextet.
 */
size_t base64_encode(const void *in, size_t len, void *out);

/***************************************************************
 * @brief Converts a sextet-encoded byte stream into a octet-encoded byte stream.
 * @param in Pointer to sextet-encoded data stream.
 * @param len Length of sextet-encoded data stream.
 * @param out Pointer to octet-encoded data stream.
 * @note @b out should be at least len * 3 / 4 bytes large.
 * @returns Length of output octet.
 */
size_t base64_decode(const void *in, size_t len, void *out);

#endif