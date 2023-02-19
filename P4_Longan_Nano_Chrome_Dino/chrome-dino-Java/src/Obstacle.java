import java.awt.*;

public interface Obstacle {
    //ͼƬ���Ͻ�(x, y)
    int getX();

    int getY();

    //ͼƬ���ƣ���x��С
    void moveLeft();

    //ͼƬ����
    int getLength();

    int getWidth();

    //����ͼƬ����
    Image getImage();

    int MOVE_SPEED = 7;
}
