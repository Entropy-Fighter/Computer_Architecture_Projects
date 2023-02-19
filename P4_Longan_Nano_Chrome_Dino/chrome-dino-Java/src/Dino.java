import javax.swing.*;
import java.awt.*;

/**
 * ��Ϸ�е�С����
 */
public class Dino {
    public static Image[] images;


    public static final int STAND_LENGTH = 42;//dinoվ��ʱͼƬ�ĳߴ�
    public static final int STAND_WIDTH = 45;

    public static final int BELOW_LENGTH = 57;//dino����ʱͼƬ�ĳߴ�
    public static final int BELOW_WIDTH = 28;


    public static final int STAND_FULL = 0;//˫���ڵ�
    public static final int LEFT_UP = 1; //����ڿ���
    public static final int RIGHT_UP = 2;//�ҽ��ڿ���
    public static final int BELOW_LEFT_UP = 3; //����ڿ��У��Ҷ���
    public static final int BELOW_RIGHT_UP = 4; //�ҽ��ڿ��У��Ҷ���


    public static final int UP = 5;//����
    public static final int DOWN = 6;//�½�

    /**
     * �����������߶�
     */
    public static final int JUMP_HEIGHT = 200;


    //dinoͼƬ�����Ͻǵ����� (x, y)
    int x = 20;//x����䣬��Ϊdinoֻ����ֱ������Ծ����Ǳ
    int y = STAND_Y;

    public static final int STAND_Y = GamePanel.groundY - (Dino.STAND_WIDTH - GamePanel.groundWidth);
    public static final int BELOW_Y = GamePanel.groundY - (Dino.BELOW_WIDTH - GamePanel.groundWidth);

    public int state = LEFT_UP;
    public int jumpSpeed = 20;
    public boolean jumping = false;//�ڿ��е�ʱ�����ٴ���


    public static final int IMAGE_NUM = 5;

    //��ͼƬ�������ڴ�
    static {
        images = new Image[IMAGE_NUM];
        images[0] = readImage("/images/Dino-stand.png");
        images[1] = readImage("/images/Dino-left-up.png");
        images[2] = readImage("/images/Dino-right-up.png");
        images[3] = readImage("/images/Dino-below-left-up.png");
        images[4] = readImage("/images/Dino-below-right-up.png");
    }

    //���ߺ���
    public static Image readImage(String fileName) {
        return Toolkit.getDefaultToolkit().getImage(
                JPanel.class.getResource(fileName));
    }

    public void jump() {
        //����Ƿ��ڿ���
        if (!(state == UP || state == DOWN))
            return;
        jumping = true;

        if (state == DOWN) {
            if (y >= GamePanel.groundY - (STAND_WIDTH - GamePanel.groundWidth)) {
                state = LEFT_UP;
                jumping = false;
            } else {
                y += jumpSpeed;
            }
        } else {//state == UP
            if (y <= GamePanel.groundY - (STAND_WIDTH - GamePanel.groundWidth) - JUMP_HEIGHT) {
                state = DOWN;
            } else {
                y -= jumpSpeed;
            }
        }
    }

    public void walk() {
        if (state == UP || state == DOWN) {
            return;
        }
        //��һֻ�ż�����·
        if (state == LEFT_UP) {
            state = RIGHT_UP;
        } else if (state == RIGHT_UP) {
            state = LEFT_UP;
        } else if (state == BELOW_LEFT_UP) {
            state = BELOW_RIGHT_UP;
        } else if (state == BELOW_RIGHT_UP) {
            state = BELOW_LEFT_UP;
        }
    }


    public void setState(int state) {
        this.state = state;
    }

    public int getLength() {
        if (state == BELOW_LEFT_UP || state == BELOW_RIGHT_UP) {
            return BELOW_LENGTH;
        }
        return STAND_LENGTH;
    }

    public int getWidth() {
        if (state == BELOW_LEFT_UP || state == BELOW_RIGHT_UP) {
            return BELOW_WIDTH;
        }
        return STAND_WIDTH;
    }

}
