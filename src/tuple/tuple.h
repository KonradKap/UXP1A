#ifndef TUPLE_TUPLE_H
#define TUPLE_TUPLE_H

/*!
 * \file src/tuple/tuple.h
 * \author Konrad Kaproń
 * \brief File containing tuple definition as well as utility functions.
 */

#define INT_TYPE 0x02
#define FLOAT_TYPE 0x01
#define STRING_TYPE 0x00
#define TYPE_MASK 0x03

#define OP_ANY 0x00
#define OP_EQ 0x04
#define OP_LT 0x08
#define OP_LE 0x10
#define OP_GT 0x20
#define OP_GE 0x40
#define OP_MASK 0xFC

#define EPSILON 10e-10

#define TUPLE_INT_FORMAT 'i'
#define TUPLE_FLOAT_FORMAT 'f'
#define TUPLE_STRING_FORMAT 's'

#define TUPLE_E_OUT_OF_RANGE -1
#define TUPLE_E_INVALID_TYPE -2
#define TUPLE_E_BAD_SIZE -3
#define TUPLE_E_INVALID_OP -4

struct tuple_element;

/*!
 * \brief tuple struct
 */
typedef struct tuple {
    unsigned nelements;
    struct tuple_element *elements;
} tuple;

/*!
 * \brief Makes tuple with fields as specified in \a format, values following.
 * \warning Pointer received by this function must be freed to avoid memory leak.
 * \param format String with characters representing types:
 *          'i' - int
 *          'f' - float
 *          's' - string
 *          Any other character is ignored.
 * \param multidot Values for each of types in \a format.
 * \return Pointer pointing to initialized \a tuple object on success, NULL otherwise.
 */
tuple *tuple_make(const char *format, ...);

/*!
 * \brief Makes tuple with given element count.
 * \warning All types are set to INT_TYPE.
 * \warning Pointer received by this function must be freed to avoid memory leak.
 * \param nelements Requested number of elements.
 * \return Pointer pointing to initialized tuple object on success, NULL otherwise.
 */
tuple *tuple_make_nelements(unsigned nelements);

/*!
 * \brief Frees memory of a given tuple.
 * \param obj Tuple to free.
 */
void tuple_free(tuple *obj);

/*!
 * \brief Returns type of a given element in a tuple.
 * \param obj Tuple.
 * \param position Index of element (0-indexed).
 * \return One of TYPE on success, one of TUPLE_E on failure.
 */
int tuple_typeof(const tuple *obj, unsigned position);

/*!
 * \brief Returns operator assosiated with given element in a tuple.
 * \param obj Tuple.
 * \param position Index of element (0-indexed).
 * \return One of OP on success, one of TUPLE_E on failure.
 */
int tuple_operator(const tuple *obj, unsigned position);

/*!
 * \brief Getter function.
 * \param obj Tuple to access.
 * \param position Index of element (0-indexed).
 * \param output Pointer to memory where the value will be written to.
 * \return 0 on success, one of TUPLE_E on failure.
 */
int tuple_get_int(const tuple *obj, unsigned position, int *output);

/*!
 * @copydoc tuple_get_int(const tuple *, unsigned, int *)
 */
int tuple_get_float(const tuple *obj, unsigned position, float *output);

/*!
 * @copydoc tuple_get_int(const tuple *, unsigned, int *)
 */
int tuple_get_string(const tuple *obj, unsigned position, char *output);

/*!
 * \brief Setter function.
 * \param obj Tuple to access.
 * \param position Index of element (0-indexed).
 * \param input Value to write.
 * \return 0 on success, one of TUPLE_E on failure.
 */
int tuple_set_int(tuple *obj, unsigned position, int input);

/*!
 * @copydoc tuple_set_int(tuple *, unsigned, int)
 */
int tuple_set_float(tuple *obj, unsigned position, float input);

/*!
 * @copydoc tuple_set_int(tuple *, unsigned, int)
 */
int tuple_set_string(tuple *obj, unsigned position, char *input);

/*!
 * @copydoc tuple_set_int(tuple *, unsigned, int)
 * \warning With OP_ANY value in input is ignored.
 * \param operator Operator to add to a blueprint.
 */
int tuple_set_int_op(tuple *obj, unsigned position, int input, unsigned short operator);

/*!
 * @copydoc tuple_set_int_op(tuple *, unsigned, int, unsigned short)
 */
int tuple_set_float_op(tuple *obj, unsigned position, float input, unsigned short operator);

/*!
 * @copydoc tuple_set_int_op(tuple *, unsigned, int, unsigned short)
 * \warning With OP_ANY string in blueprint is set to "".
 */
int tuple_set_string_op(tuple *obj, unsigned position, char *input, unsigned short operator);

/*!
 * \brief Compare tuple to a blueprint
 * \param obj Tuple.
 * \param blueprint Blueprint.
 * \return 1 if obj meets requirements in blueprint, otherwise 0, one of TUPLE_E on error.
 * \see tuple_compare_test*
 */
int tuple_compare_to(const tuple* obj, const tuple *blueprint);

/*!
 * \brief Serializes tuple to given buffer.
 * \param obj Tuple.
 * \param buffer Memory where the tuple will be serialized.
 * \param size Length of \a buffer.
 * \return 0 on success, one of TUPLE_E on failure.
 */
int tuple_to_buffer(const tuple *obj, char *buffer, int size);

/*!
 * \brief Makes tuple from serialized data in buffer.
 * \warning Pointer received by this function must be freed to avoid memory leak.
 * \param buffer Memory to read.
 * \return Pointer to valid tuple object on success, NULL otherwise.
 */
tuple *tuple_from_buffer(const char *buffer);

#endif //TUPLE_TUPLE_H
