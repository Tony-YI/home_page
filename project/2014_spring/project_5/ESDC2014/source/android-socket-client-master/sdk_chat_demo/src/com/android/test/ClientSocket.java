/* 
 * PC与Android文件传输 
 * Android客户端 
 */  
package com.android.test;  
import java.io.BufferedInputStream;  
import java.io.DataInputStream;  
import java.io.DataOutputStream;  
import java.io.IOException;  
import java.net.InetSocketAddress;
import java.net.Socket;  
import java.net.UnknownHostException;  

import android.util.Log;
public class ClientSocket {  
    private String ip;  
    private int port;  
    private Socket socket2 = null;  
    DataOutputStream out = null;  
    DataInputStream getMessageStream = null;  
    public ClientSocket(String ip, int port) {  
        this.ip = ip;  
        this.port = port;  
    }  
    public void createConnection() {  
        try {  
        	Log.v("1111","1111");
        	socket2 = new Socket();  
        	 Log.v("2222","2222");
        	socket2.connect(new InetSocketAddress(ip, port), 15*1000);
            Log.v("3333","3333");
        } catch (UnknownHostException e) {  
            // TODO Auto-generated catch block  
            e.printStackTrace();  
            if (socket2 != null) {  
                try {  
                	socket2.close();  
                } catch (IOException e1) {  
                    // TODO Auto-generated catch block  
                    e1.printStackTrace();  
                }  
            }  
        } catch (IOException e) {  
            // TODO Auto-generated catch block  
            e.printStackTrace();  
            if (socket2 != null) {  
                try {  
                	socket2.close();  
                } catch (IOException e1) {  
                    // TODO Auto-generated catch block  
                    e1.printStackTrace();  
                }  
            }  
        } finally {  
        }  
    }  
    public void sendMessage(String sendMessage) {  
        try {  
            out = new DataOutputStream(socket2.getOutputStream());  
            if (sendMessage.equals("Windows")) {  
                out.writeByte(0x1);  
                out.flush();  
                return;  
            }  
            if (sendMessage.equals("Unix")) {  
                out.writeByte(0x2);  
                out.flush();  
                return;  
            }  
            if (sendMessage.equals("Linux")) {  
                out.writeByte(0x3);  
                out.flush();  
            } else {  
                out.writeUTF(sendMessage);  
                out.flush();  
            }  
        } catch (IOException e) {  
            // TODO Auto-generated catch block  
            e.printStackTrace();  
            if (out != null) {  
                try {  
                    out.close();  
                } catch (IOException e1) {  
                    // TODO Auto-generated catch block  
                    e1.printStackTrace();  
                }  
            }  
        }  
    }  
    public DataInputStream getMessageStream() {  
        try {  
            getMessageStream = new DataInputStream(new BufferedInputStream(  
            		socket2.getInputStream()));  
            // return getMessageStream;  
        } catch (IOException e) {  
            // TODO Auto-generated catch block  
            e.printStackTrace();  
            if (getMessageStream != null) {  
                try {  
                    getMessageStream.close();  
                } catch (IOException e1) {  
                    // TODO Auto-generated catch block  
                    e1.printStackTrace();  
                }  
            }  
        }  
        return getMessageStream;  
    }  
    public void shutDownConnection() {  
        try {  
            if (out != null) {  
                out.close();  
            }  
            if (getMessageStream != null) {  
                getMessageStream.close();  
            }  
            if (socket2 != null) {  
            	socket2.close();  
            }  
        } catch (IOException e) {  
            // TODO Auto-generated catch block  
            e.printStackTrace();  
        }  
    }  
}  