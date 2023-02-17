/* compile with
 *
 * 	gcc -g -Wall try208.c `pkg-config vips --cflags --libs`
 */

#include <vips/vips.h>

/* Create a tiny VipsOperator ... photographic negative of a uchar image.
 */

typedef struct _Negative {
	VipsOperation parent_instance;

	VipsImage *in;
	VipsImage *out;

	int image_max;

} Negative;

typedef struct _NegativeClass {
	VipsOperationClass parent_class;

	/* No new class members needed for this op.
	 */

} NegativeClass;

/* This defines a function called negative_get_type() which adds the new
 * operation to the class hierarchy, and negative_parent_class, our
 * superclass.
 *
 * You must define negative_class_init() to init a new class struct and
 * negative_init() to init a new instance struct.
 */

G_DEFINE_TYPE( Negative, negative, VIPS_TYPE_OPERATION );

static int
negative_generate( VipsRegion *or,
	void *vseq, void *a, void *b, gboolean *stop )
{
	/* The area of the output region we have neen asked to make.
	 */
	VipsRect *r = &or->valid;

	/* The sequence value ... the thing returned by start_one.
	 */
	VipsRegion *ir = (VipsRegion *) vseq;

	Negative *negative = (Negative *) b;
	int line_size = r->width * negative->in->Bands;

	int x, y;

	/* Request matching part of input region.
	 */
	if( vips_region_prepare( ir, r ) )
		return( -1 );

	for( y = 0; y < r->height; y++ ) {
		unsigned char *p = (unsigned char *)
			VIPS_REGION_ADDR( ir, r->left, r->top + y );
		unsigned char *q = (unsigned char *)
			VIPS_REGION_ADDR( or, r->left, r->top + y );

		for( x = 0; x < line_size; x++ )
			q[x] = negative->image_max - p[x];
	}

	return( 0 );
}

static int
negative_build( VipsObject *object )
{
	VipsObjectClass *class = VIPS_OBJECT_GET_CLASS( object );
	Negative *negative = (Negative *) object;

	/* Build all the superclasses. This will set @in and
	 * @image_max.
	 */
	if( VIPS_OBJECT_CLASS( negative_parent_class )->build( object ) )
		return( -1 );

	/* We only work for uchar images.
	 */
	if( vips_check_uncoded( class->nickname, negative->in ) ||
		vips_check_format( class->nickname,
			negative->in, VIPS_FORMAT_UCHAR ) )
		return( -1 );

	/* Get ready to write to @out. @out must be set via g_object_set() so
	 * that vips can see the assignment. It'll complain that @out hasn't
	 * been set otherwise.
	 */
	g_object_set( object, "out", vips_image_new(), NULL );

	/* Tell vips that @out depends on @in and prefers to work in
	 * scanlines.
	 */
	if( vips_image_pipelinev( negative->out,
		VIPS_DEMAND_STYLE_THINSTRIP, negative->in, NULL ) )
		return( -1 );

	/* This attaches callbacks to @out to calculate pixels on request.
	 * start_one and stop_one are a pair of start and stop functions which
	 * create a single input region on extra argument 1, handy for 1-ary
	 * operations.
	 */
	if( vips_image_generate( negative->out,
		vips_start_one,
		negative_generate,
		vips_stop_one,
		negative->in, negative ) )
		return( -1 );

	return( 0 );
}

static void
negative_class_init( NegativeClass *class )
{
	GObjectClass *gobject_class = G_OBJECT_CLASS( class );
	VipsObjectClass *object_class = (VipsObjectClass *) class;

	gobject_class->set_property = vips_object_set_property;
	gobject_class->get_property = vips_object_get_property;

	object_class->nickname = "negative";
	object_class->description = "photographic negative";
	object_class->build = negative_build;

	VIPS_ARG_IMAGE( class, "in", 1,
		"Input",
		"Input image",
		VIPS_ARGUMENT_REQUIRED_INPUT,
		G_STRUCT_OFFSET( Negative, in ) );

	VIPS_ARG_IMAGE( class, "out", 2,
		"Output",
		"Output image",
		VIPS_ARGUMENT_REQUIRED_OUTPUT,
		G_STRUCT_OFFSET( Negative, out ) );

	VIPS_ARG_INT( class, "image_max", 4,
		"Image maximum",
		"Maximum value in image: pivot about this",
		VIPS_ARGUMENT_OPTIONAL_INPUT,
		G_STRUCT_OFFSET( Negative, image_max ),
		0, 255, 255 );

}

static void
negative_init( Negative *negative )
{
	negative->image_max = 255;
}

/* A type-safe way to call nagative from C.
 *
 * You can also use
 *
 * 	vips_call( "negative", in, &out, "image_max", 128, NULL )
 *
 * but that won't have any compile-time checks, of course.
 */
G_MODULE_EXPORT int
negative( VipsImage *in, VipsImage **out, ... )
{
	va_list ap;
	int result;

	va_start( ap, out );
	result = vips_call_split( "negative", ap, in, out );
	va_end( ap );

	return( result );
}

G_MODULE_EXPORT const gchar *
g_module_check_init( GModule *module )
{
	printf( "vips_negative: module init\n" );

	negative_get_type();

	/* We can't be unloaded, there would be chaos.
	 */
	g_module_make_resident( module );

	return( NULL );
}
