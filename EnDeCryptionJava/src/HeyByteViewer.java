import javax.swing.*;
import javax.swing.text.AttributeSet;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Random;
import java.util.StringJoiner;

/**
 * Created by haris on 08.07.16.
 */
public class HeyByteViewer extends JFrame {
    JButton btnShowFile;
    JTextPane txtByteField;
    JScrollPane scroll;

    public HeyByteViewer() {
        setLayout(null);
        setTitle("Hex Byte Viewer");
        setSize(700, 500);
        setLocationRelativeTo(null);
        setVisible(true);

        btnShowFile = new JButton();
        btnShowFile.setText("Load File");
//        btnShowFile.setLocation(10, 10);
//        btnShowFile.setVisible(true);
        btnShowFile.setBounds(10, 10, 100, 50);
        btnShowFile.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                btnShowFileAction(e);
            }
        });
        add(btnShowFile);

        txtByteField = new JTextPane();
        txtByteField.setBounds(10, 70, 600, 340);
        txtByteField.setFont(new Font("courier", 0, 16));
        txtByteField.setEditable(true);

        scroll = new JScrollPane(txtByteField);
        scroll.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
        scroll.setBounds(10, 70, 600, 340);
        scroll.setBorder(BorderFactory.createLineBorder(Color.black));
        add(scroll);

        setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
    }

    public void btnShowFileAction(ActionEvent e) {
        int length = 323;
        // Should be readed from a file
        byte[] bytes = getRandomBytes(length);
        String hexViewString = getHexViewStringFromBytes(bytes, this.txtByteField);

//        txtByteField.setText(hexViewString);
        txtByteField.setCaretPosition(0);
    }

    public byte[] getRandomBytes(int length) {
        byte[] bytes = new byte[length];
        Random rand = new Random();

        for (int i = 0; i < length; i++) {
            bytes[i] = (byte)(rand.nextInt() & 0xFF);
        }

        return bytes;
    }

    String getHexViewStringFromBytes(byte[] bytes, JTextPane txtPane) {
        String hexViewString = "";
        int length = bytes.length;
        Color colorTop  = new Color(0xA0, 0xA0, 0x10);
        Color colorLeft = new Color(0xC0, 0x30, 0x60);
        Color color1   = new Color(0x40, 0x40, 0x80);
        Color color2   = new Color(0x40, 0x80, 0x40);
        txtPane.setText("");

        int extraSpace = 0;
        int temp = length;
        while (temp > 0) {
            temp >>= 4;
            extraSpace++;
        }

        for (int i = 0; i < 2+extraSpace; i++) {
            hexViewString += " ";
            appendToPane(txtPane, " ", colorTop);
        }

        for (int i = 0; i < 0x10; i++) {
            hexViewString += String.format(" %02X", (0xFF & i));
            appendToPane(txtPane, String.format(" %02X", (0xFF & i)), colorTop);
        }
        hexViewString += "\n";
        appendToPane(txtPane, "\n", Color.DARK_GRAY);

        int lines = length / 0x10;
        int rest = length % 0x10;

        String formatDigits = String.format("0x%%0%dX", extraSpace);
        for (int y = 0; y < lines; y++) {
            hexViewString += String.format(formatDigits, y*0x10);
            appendToPane(txtPane, String.format(formatDigits, y*0x10), colorLeft);
            for (int x = 0; x < 0x10; x++) {
                hexViewString += String.format(" %02X", bytes[y*0x10+x]);
                appendToPane(txtPane, String.format(" %02X", bytes[y*0x10+x]), (x%2==0?color1:color2));
            }
            hexViewString += "\n";
            appendToPane(txtPane, "\n", Color.DARK_GRAY);
        }

        if (rest > 0) {
            hexViewString += String.format(formatDigits, lines*0x10);
            appendToPane(txtPane, String.format(formatDigits, lines*0x10), colorLeft);
            for (int x = 0; x < rest; x++) {
                hexViewString += String.format(" %02X", bytes[lines*0x10+x]);
                appendToPane(txtPane, String.format(" %02X", bytes[lines*0x10+x]), (x%2==0?color1:color2));
            }
        }

        return hexViewString;
    }

    private void appendToPane(JTextPane tp, String msg, Color c)
    {
        StyleContext sc = StyleContext.getDefaultStyleContext();
        AttributeSet aset = sc.addAttribute(SimpleAttributeSet.EMPTY, StyleConstants.Foreground, c);

        aset = sc.addAttribute(aset, StyleConstants.FontFamily, "courier");//"Lucida Console");
        aset = sc.addAttribute(aset, StyleConstants.Alignment, StyleConstants.ALIGN_JUSTIFIED);

        int len = tp.getDocument().getLength();
        tp.setCaretPosition(len);
        tp.setCharacterAttributes(aset, false);
        tp.replaceSelection(msg);
    }

    public static void main(String[] args) {
        HeyByteViewer hexByteViewer = new HeyByteViewer();
    }
}
