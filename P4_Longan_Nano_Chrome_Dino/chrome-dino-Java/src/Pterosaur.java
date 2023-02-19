import javax.swing.*;
import java.awt.*;

/**
 * ��Ϸ�е�����
 */
public class Pterosaur implements Obstacle {

    public static Image[] images;

    public static final int IMAGE_NUM = 2;

    public static int picWidth = 44;
    public static int picLength = 47;

    //��ͼƬ�������ڴ�
    static {
        images = new Image[IMAGE_NUM];
        images[0] = readImage("/images/Pterosaur-up.png");
        images[1] = readImage("/images/Pterosaur-down.png");
    }

    //���ߺ���
    public static Image readImage(String fileName) {
        return Toolkit.getDefaultToolkit().getImage(
                JPanel.class.getResource(fileName));
    }

    public int x;
    public int y;
    public int state;
    public static final int WING_UP = 0;
    public static final int WING_DOWN = 1;

    public Pterosaur() {
        state = WING_UP;
        x = GameView.FRAME_LENGTH;
        y = GamePanel.groundY + GamePanel.groundWidth - picWidth  - (int) (50 * Math.random());
    }

    @Override
    public int getX() {
        return x;
    }

    @Override
    public int getY() {
        return y;
    }

    @Override
    public void moveLeft() {
        x -= Obstacle.MOVE_SPEED;
    }

    @Override
    public int getLength() {
        return picLength;
    }

    @Override
    public int getWidth() {
        return picWidth;
    }

    @Override
    public Image getImage() {
        if (state == WING_UP) {
            state = WING_DOWN;
        } else {
            state = WING_UP;
        }
        return images[state];
    }
}
