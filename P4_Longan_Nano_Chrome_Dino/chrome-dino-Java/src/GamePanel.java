import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.ArrayList;

public class GamePanel extends JPanel implements KeyListener, Runnable {

    private int score = 0;

    private Image ground;
    public static int groundY = 400;//����������ϵ��y����
    public static int groundX = 0;//����������ϵ��x����
    public static int groundWidth = 5;//����ͼƬ�Ŀ���

    private long lastObstacleCreated = System.currentTimeMillis();
    private long obstacleCreatedInterval = 1700;//������ ���� ����һ���ϰ���
    private static final int OBSTACLE_NUM = 3;//һ���м����ϰ���

    private int gameState;

    private static final int START_MENU = 0;
    private static final int GAMING = 1;
    private static final int END_MENU = 2;

    private Dino dino;
    private ArrayList<Obstacle> obstacles = new ArrayList<>();

    public GamePanel() {
        gameState = START_MENU;
        ground = Toolkit.getDefaultToolkit().getImage(
                JPanel.class.getResource("/images/Ground.png"));
        dino = new Dino();

    }

    @Override
    public void paint(Graphics g) {
        super.paint(g);

        if (gameState == START_MENU) {
            paintStartMenu(g);
        } else if (gameState == GAMING) {
            paintGround(g);
            paintDino(g);
            paintScore(g);
            paintObstacle(g);
        } else if (gameState == END_MENU) {
            paintEndMenu(g);
        }
    }

    private void paintGround(Graphics g) {
        g.drawImage(ground, groundX, groundY, 1200, 12, this);
        groundX -= Obstacle.MOVE_SPEED;//���ϰ����ƶ��ٶ�һ��
        if (groundX + 1200 < GameView.FRAME_LENGTH) groundX = 0;
    }

    private void paintDino(Graphics g) {
        int picIndex = dino.state;

        //���dino�ڿ��У�ʹ�õ�ͼƬ��վ�ڵ��ϵ���ͬһ��
        if (dino.state == Dino.UP || dino.state == Dino.DOWN) {
            picIndex = Dino.STAND_FULL;
        }
        int picLength = Dino.STAND_LENGTH;
        int picWidth = Dino.STAND_WIDTH;

        //dino���º�վ��ʱ��ͼƬ�ߴ��ǲ�һ����
        if (dino.state == Dino.BELOW_LEFT_UP || dino.state == Dino.BELOW_RIGHT_UP) {
            picLength = Dino.BELOW_LENGTH;
            picWidth = Dino.BELOW_WIDTH;
        }

        //stateͬʱ����ʹ�õĵ�ͼƬ���±�
        g.drawImage(Dino.images[picIndex], dino.x, dino.y, picLength, picWidth, this);
    }

    private void paintScore(Graphics g) {
        g.setColor(Color.black);

        g.setFont(new Font("����", Font.BOLD, 25));
        g.drawString(score + "", GameView.FRAME_LENGTH - 100, 50);
    }

    private void paintObstacle(Graphics g) {

        for (int i = 0; i < obstacles.size(); i++) {
            Obstacle obstacle = obstacles.get(i);

            if (obstacle.getX() <= 0) {
                obstacles.remove(i);
                i--;//ʹ������ɾ��Ԫ�ص�ʱ��Ҫע��
                continue;
            }

            Image image = obstacle.getImage();
            g.drawImage(image, obstacle.getX(), obstacle.getY(),
                    obstacle.getLength(), obstacle.getWidth(), this);
            obstacle.moveLeft();
        }
    }

    private void paintStartMenu(Graphics g) {
        g.setColor(Color.black);

        g.setFont(new Font("����", Font.BOLD, 50));
        g.drawString("Chrome Dino", GameView.FRAME_LENGTH / 4, GameView.FRAME_WIDTH / 3);
        g.setFont(new Font("����", Font.BOLD, 25));

        g.drawString("press space to play", GameView.FRAME_LENGTH / 4, GameView.FRAME_WIDTH / 2);
    }


    private void paintEndMenu(Graphics g) {
        g.setColor(Color.black);

        g.setFont(new Font("����", Font.BOLD, 50));
        g.drawString("Good Game", GameView.FRAME_LENGTH / 4, GameView.FRAME_WIDTH / 3);
        g.setFont(new Font("����", Font.BOLD, 25));

        g.drawString(String.format("Score is %d", score), GameView.FRAME_LENGTH / 4, GameView.FRAME_WIDTH / 2);
        g.drawString("press space to play a new round", GameView.FRAME_LENGTH / 4, GameView.FRAME_WIDTH / 2 + 60);
    }


    @Override
    public void keyTyped(KeyEvent e) {

    }

    //���°����������¼�
    @Override
    public void keyPressed(KeyEvent e) {

        if (gameState == GAMING) {

            //dino �ڿ���ʱ�������κβ���
            if (dino.jumping) {
            } else if (e.getKeyCode() == KeyEvent.VK_DOWN) {
                //dino�¶�

                dino.setState(Dino.BELOW_LEFT_UP);
                dino.y = Dino.BELOW_Y;
            } else if (e.getKeyCode() == KeyEvent.VK_SPACE) {
                //dino����
                dino.setState(Dino.UP);
            }
        } else if (gameState == START_MENU) {
            //���ո����ʼ��Ϸ
            if (e.getKeyCode() == KeyEvent.VK_SPACE) {
                gameState = GAMING;
            }
        } else if (gameState == END_MENU) {
            //���ո����ʼ��һ����Ϸ
            if (e.getKeyCode() == KeyEvent.VK_SPACE) {
                resetGame();
            }
        }
//        repaint();
    }

    //�ͷŰ����������¼�
    @Override
    public void keyReleased(KeyEvent e) {
        if (gameState == GAMING) {

            //dino �ڿ���ʱ�������κβ���
            if (dino.jumping) {
            } else if (e.getKeyCode() == KeyEvent.VK_DOWN) {
                //dino���ٶ���
                dino.setState(Dino.LEFT_UP);
                dino.y = Dino.STAND_Y;

            }
        }
//        repaint();
    }

    @Override
    public void run() {
        while (true) {
            try {
                Thread.sleep(30);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            if (gameState == GAMING) {
                createObstacle();
                dino.walk();
                dino.jump();
                score++;
                checkCollision();
            }

            repaint();
        }
    }


    private void createObstacle() {
        if (System.currentTimeMillis() < lastObstacleCreated + obstacleCreatedInterval) {
            return;
        }
        lastObstacleCreated = System.currentTimeMillis();

        //����[0, 1)�������
        double choice = Math.random();

        if (choice < 1 / (double) OBSTACLE_NUM) {
            obstacles.add(new Cactus(0));
        } else if (choice < 2 / (double) OBSTACLE_NUM) {
            obstacles.add(new Cactus(1));
        } else if (choice < 3 / (double) OBSTACLE_NUM) {
            obstacles.add(new Pterosaur());
        }
    }

    /**
     * ���dino�Ƿ�������ϰ�����ײ
     */
    private void checkCollision() {
        for (Obstacle obstacle : obstacles) {
            if (collided(obstacle)) {
                gameState = END_MENU;
                break;
            }
        }
    }

    /**
     * ���dino�Ƿ��ĳһ�ϰ�����ײ
     *
     * @param obstacle ���Ƚϵ��ϰ���
     * @return �����ײ������true,���򷵻�false
     */
    private boolean collided(Obstacle obstacle) {
        int x = obstacle.getX();
        int y = obstacle.getY();
        int length = obstacle.getLength();
        int width = obstacle.getWidth();

        // ���ķ����ǿ��ϰ���ͼƬ���ĸ��ǵĵ��Ƿ���dinoͼƬ��
        if (pointInDino(x, y)) return true;
        else if (pointInDino(x + length, y)) return true;
        else if (pointInDino(x + length, y + width)) return true;
        else if (pointInDino(x, y + width)) return true;

        return false;
    }

    /**
     * (x, y)�Ƿ���dinoͼƬ��
     */
    private boolean pointInDino(int x, int y) {
        return dino.x <= x && x <= dino.x + dino.getLength() &&
                dino.y <= y && y <= dino.y + dino.getWidth();
    }

    //��������������³�ʼ��
    private void resetGame() {
        score = 0;
        gameState = START_MENU;
        obstacles.clear();
    }
}
