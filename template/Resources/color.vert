uniform mediump mat4 MVP;
attribute mediump vec4 POSITION;
attribute lowp vec4 COLOR;

varying lowp vec4 color;

void main(void) {
	gl_Position = MVP * POSITION;
	color = COLOR;
}