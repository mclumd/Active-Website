/*  Daniel Wyszynski 
    Center for Applied Large-Scale Computing (CALC) 
    04-12-95 

    Test of text animation.

    kwalrath: Changed string; added thread suspension. 5-9-95
*/
import java.awt.Graphics;
import java.awt.Color;
import java.awt.Font;
import java.awt.Component;

public class NervousText extends java.applet.Applet implements Runnable {

	char separated[];
	String s = null;
	Thread killme = null;
	int i;
	int x_coord = 0, y_coord = 0;
	String num;
	int speed=35;
	int counter =0;
	Color color = new Color(0, 0, 0);
	Color back = new Color(255,255,255);
	boolean threadSuspended = false; //added by kwalrath

public void init() {
	setBackground(back);
	resize(300,75);
	setFont(new Font("Helvetica",Font.BOLD,40));
	s = getParameter("text");
	if (s == null) {
	    s = "HotJava";
	}

	separated =  new char [s.length()];
	s.getChars(0,s.length(),separated,0);
 }

public void start() {
	if(killme == null) 
	{
        killme = new Thread(this);
        killme.start();
	}
 }

public void stop() {
	killme = null;
 }

public void run() {
	while (killme != null) {
	try {Thread.sleep(100);} catch (InterruptedException e){}
	repaint();
	}
	killme = null;
 }

public void paint(Graphics g) {
	g.setColor(color);
	for(i=0;i<s.length();i++)
	{
	x_coord = (int) (Math.random()*20+20*i);
	y_coord = (int) (Math.random()*15+36);
	g.drawChars(separated, i,1,x_coord,y_coord);
	}
 }
 
/* Added by kwalrath. */
public boolean mouseDown(java.awt.Event evt, int x, int y) {
        if (threadSuspended) {
            killme.resume();
        }
        else {
            killme.suspend();
        }
        threadSuspended = !threadSuspended;
    return true;
    }
}

