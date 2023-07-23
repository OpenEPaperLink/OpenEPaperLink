package com.github.sandeepmistry.arduino.nRF5;

import cc.arduino.packages.Uploader;
import cc.arduino.packages.uploaders.SerialUploader;

import java.awt.Dimension;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.File;
import java.io.FileInputStream;

import java.lang.Runnable;
import java.lang.Thread;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

import java.net.URL;

import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;

import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipEntry;

import processing.app.Editor;
import processing.app.PreferencesData;
import processing.app.tools.Tool;

public class nRF5FlashSoftDevice implements Tool {
  private Map<String, String> urls = new HashMap<String, String>();
  private Map<String, String> filenames = new HashMap<String, String>();
  private Editor editor;

  public void init(Editor editor) {
    this.editor = editor;
  }

  public String getMenuTitle() {
    return "nRF5 Flash SoftDevice";
  }

  public void run() {
    if (!PreferencesData.get("target_platform").equals("nRF5")) {
      editor.statusError(getMenuTitle() + " is only supported on 'Nordic Semiconductor nRF5 Boards' boards!");
      return;
    }

    Path softdeviceTxtPath = Paths.get(PreferencesData.get("runtime.platform.path"), "softdevices.txt");
    Properties softdevices = new Properties();

    try {
      softdevices.load(new FileInputStream(softdeviceTxtPath.toString()));
    } catch (Exception e) {
      editor.statusError("Error while flashing SoftDevice.");
      System.err.println(e);
      return;
    }

    String[] names = softdevices.getProperty("names").split(",");

    for (int i = 0; i < names.length; i++) {
      String name = names[i];
      String url = softdevices.getProperty(name + ".url");
      String filename = softdevices.getProperty(name + ".filename");

      urls.put(name, url);
      filenames.put(name, filename);
    }

    String softDevice = PreferencesData.get("custom_softdevice");
    if (softDevice == null || softDevice.endsWith("none")) {
      editor.statusError("No SoftDevice selected!");
      return;
    }

    String programmer = PreferencesData.get("programmer");
    if (programmer == null || !programmer.startsWith("sandeepmistry:")) {
      editor.statusError("Unsupported programmer!");
      return;
    }

    softDevice = softDevice.substring(softDevice.lastIndexOf("_") + 1);
    String url = urls.get(softDevice);

    if (url == null) {
      editor.statusError("Unsupported SoftDevice!");
      return;
    }

    Path softdevicePath = Paths.get(PreferencesData.get("runtime.platform.path"), "cores", "nRF5", "SDK", "components", "softdevice", softDevice, "hex");
    Path softdeviceHexFile = Paths.get(softdevicePath.toString(), filenames.get(softDevice));

    Runnable runnable = () -> {
      try {
        if (Files.notExists(softdeviceHexFile)) {
          System.out.println("Downloading '" +url + "' ...");

          InputStream is = new URL(url).openStream();
          ByteArrayOutputStream outputStream = new ByteArrayOutputStream();

          byte[] chunk = new byte[32 * 1024];
          int bytesRead;

          while ((bytesRead = is.read(chunk)) > 0) {
            System.out.print(".");
            outputStream.write(chunk, 0, bytesRead);
          }

          System.out.println("done");

          ZipInputStream zis = new ZipInputStream(new ByteArrayInputStream(outputStream.toByteArray()));
          ZipEntry entry;

          String licenseAgreementFilename = null;
          StringBuilder licenseAgreementBuilder = new StringBuilder();

          String softdeviceFilename = null;
          StringBuilder softdeviceBuilder = new StringBuilder();

          while ((entry = zis.getNextEntry()) != null) {
            if (entry.getName().endsWith("agreement.txt")) {
              licenseAgreementFilename = entry.getName();

              for (int i = 0; i < entry.getSize(); i++) {
                licenseAgreementBuilder.append((char)zis.read());
              }
            } else if (entry.getName().endsWith("softdevice.hex")) {
              softdeviceFilename = entry.getName();

              for (int i = 0; i < entry.getSize(); i++) {
                softdeviceBuilder.append((char)zis.read());
              }
            }
          }

          if (licenseAgreementFilename != null) {
            JTextArea textArea = new JTextArea(licenseAgreementBuilder.toString());
            textArea.setColumns(80);
            textArea.setRows(50);
            textArea.setLineWrap(true);
            textArea.setWrapStyleWord(true);
            textArea.setSize(textArea.getPreferredSize().width, 1);

            JScrollPane scrollPane = new JScrollPane(textArea);
            scrollPane.setPreferredSize( new Dimension(textArea.getPreferredSize().width, 500 ) );

            int result = JOptionPane.showOptionDialog(null, scrollPane, "NORDIC SEMICONDUCTOR ASA SOFTDEVICE LICENSE AGREEMENT", JOptionPane.DEFAULT_OPTION, JOptionPane.QUESTION_MESSAGE, null, new Object[]{"Accept", "Decline"}, "Decline");

            if (result != 0) {
              return;
            }

            Files.write(Paths.get(softdevicePath.toString(), licenseAgreementFilename), String.valueOf(licenseAgreementBuilder).getBytes());
          }

          if (softdeviceFilename != null) {
            Files.write(Paths.get(softdevicePath.toString(), softdeviceFilename), String.valueOf(softdeviceBuilder).getBytes());
          }
        }

        Uploader uploader = new SerialUploader();
        if (uploader.burnBootloader()) {
          editor.statusNotice("Done flashing SoftDevice.");
        } else {
          editor.statusError("Error while flashing SoftDevice.");
        }
      } catch (Exception e) {
        editor.statusError("Error while flashing SoftDevice.");
        System.err.println(e);
        return;
      }
    };

    Thread thread = new Thread(runnable);
    thread.start();
  }
}
