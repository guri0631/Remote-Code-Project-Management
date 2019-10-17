///////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - GUI for  Project4                            //
// ver 1.0                                                           //
//Arjun Kalyanasundaram, CSE687 - Object Oriented Design, Spring 2018//
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package provides a WPF-based GUI for Project4 .  It's 
 * responsibilities are to:
 * - Provide a display of directory contents of a remote ServerPrototype.
 * - It provides a subdirectory list and a filelist for the selected directory.
 * - You can navigate into subdirectories by double-clicking on subdirectory
 *   or the parent directory, indicated by the name "..".
 *   It also provides for the conversion of files and can also view the files 
 *   sent to the Client, in the Client Files Tab.
 * Required Files:
 * ---------------
 * Mainwindow.xaml, MainWindow.xaml.cs
 * Translater.dll
 * 
 * Maintenance History:
 * --------------------
 * ver 1.0 : 30 April 2019
 * - first release
 */

// Translater has to be statically linked with CommLibWrapper
// - loader can't find Translater.dll dependent CommLibWrapper.dll
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Threading;
using MsgPassingCommunication;
using System.IO;


namespace WpfApp1
{
    public partial class MainWindow : Window
    {
        string workingDir = "../../../ClientFolder";
        private string WDirectory { get; set; }
        private string Patterns { get; set; }
        private string Regex { get; set; }
        public MainWindow()
        {
            InitializeComponent();
        }

        private Stack<string> pathStack_ = new Stack<string>();
        private Translater translater;
        private CsEndPoint endPoint_;
        private Thread rcvThrd = null;
        private Dictionary<string, Action<CsMessage>> dispatcher_
          = new Dictionary<string, Action<CsMessage>>();

        //----< process incoming messages on child thread >----------------
        private void processMessages()
        {
            ThreadStart thrdProc = () => {
                while (true)
                {
                    CsMessage msg = translater.getMessage();
                    string msgId = msg.value("command");
                    if (dispatcher_.ContainsKey(msgId))
                        dispatcher_[msgId].Invoke(msg);
                }
            };
            rcvThrd = new Thread(thrdProc);
            rcvThrd.IsBackground = true;
            rcvThrd.Start();
        }
        //----< fucntion to clear files >----------------
        private void clearFiles()
        {
            listConverted.Items.Clear();
        }
        //----< function dispatched by child thread to main thread >-------

        private void addFile(string file)
        {
            listConverted.Items.Add(file);
        }
        //----< add client processing for message with key >---------------

        private void addClientProc(string key, Action<CsMessage> clientProc)
        {
            dispatcher_[key] = clientProc;
        }

        //----<sends message to convert files >---------------
        private void DispatchconvertFiles()
        {
            Action<CsMessage> convert = (CsMessage rcvMsg) =>
            {
                Action clrFiles = () =>
                {
                    clearFiles();
                };
                Dispatcher.Invoke(clrFiles, new Object[] { });
                var enumer = rcvMsg.attributes.GetEnumerator();
                while (enumer.MoveNext())
                {
                    string key = enumer.Current.Key;
                    string absolutePath = Directory.GetCurrentDirectory();
                    if (key.Contains("conver"))
                    {
                        Action<string> doFile = (string conver) =>
                        {
                            addFile(conver);
                        };
                        Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
                    }
                }

            };
            addClientProc("convert", convert);          //message to convert files 
        }

        //----<function to load the dispactcher >---------------
        private void loadDispatcher()
        {
            DispatchconvertFiles();
        }
        //----< input for GUI  to be used in Loaded Window>------
        private void input(List<string> listnr)
        {
            listnr.Add("WpfApp1");
            Patterns = "*.h *.cpp";
            Regex = "[A-B](.*)";
            txtRegex.Text = Regex;
            txtPatterns.Text = Patterns;
            txtpath.Text = WDirectory;
            listnr.Add(WDirectory);
            listnr.Add("/s");
            listnr.AddRange(Patterns.Split(' '));
            listnr.AddRange(Regex.Split(' '));
        }
        //----<Function To demonstrate getfile  >----------------    
        private void demo_getfile(CsEndPoint serverEndPointn)
        {
            CsMessage msg_get = new CsMessage();
            msg_get.add("to", CsEndPoint.toString(serverEndPointn));
            msg_get.add("from", CsEndPoint.toString(endPoint_));
            msg_get.add("command", "getFile");
            msg_get.add("name", "AbstrSynTree.cpp.html");
            translater.postMessage(msg_get);
            msg_get.attributes["name"] = "AbstrSynTree.h.html";
            translater.postMessage(msg_get);
            string webpagepath = "AbstrSynTree.h.html";
            Clientlist.Items.Add("AbstrSynTree.h.html");
            string changePath = "..\\..\\..\\ConvertedWebpages\\";
            changePath = changePath + webpagepath;
            System.Diagnostics.Process.Start(changePath);

        }
   //----< start Comm, fill window display with files  to convert>------
 private void Window_Loaded(object sender, RoutedEventArgs e)
    {
          if (!System.IO.Directory.Exists(workingDir))
              System.IO.Directory.CreateDirectory(workingDir);
            endPoint_ = new CsEndPoint();
            endPoint_.machineAddress = "localhost";
            endPoint_.port = 8082;
            translater = new Translater();
            translater.listen(endPoint_, workingDir, workingDir);
            processMessages(); 
            loadDispatcher();
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            pathStack_.Push("../Storage");
            CsMessage messg = new CsMessage();
            messg.add("to", CsEndPoint.toString(serverEndPoint));
            messg.add("from", CsEndPoint.toString(endPoint_));
            messg.add("command", "getDirs");
            messg.add("path", pathStack_.Peek());
            translater.postMessage(messg);
            messg.remove("command");
            messg.add("command", "getFiles");
            translater.postMessage(messg);
            string[] args = Environment.GetCommandLineArgs();
            WDirectory = "../../../";
            WDirectory = Path.GetFullPath(WDirectory);
            List<string> list_st = new List<string>();
            input(list_st);                                      // input for arguments 
            CsMessage msg_Conv = new CsMessage();
            msg_Conv.add("to", CsEndPoint.toString(serverEndPoint));     //sending message to convert files
            msg_Conv.add("from", CsEndPoint.toString(endPoint_));
            msg_Conv.add("command", "convert");
            msg_Conv.add("convert", args.First());
            msg_Conv.add("argc", list_st.Count().ToString());
            int count  = 1;
            string cmd = "cmd";
           foreach( string val_store in list_st)
            {
             string listch = cmd + count;
                msg_Conv.add(listch, val_store.ToString());
             count++;
            }
            translater.postMessage(msg_Conv);
            translater.getMessage();
            demo_getfile(serverEndPoint);                // demonstrating file sent to clientFolder
        }

        
    //----< strip off name of first part of path >---------------------
    private string removeFirstDir(string path)
    {
      string modifiedPath = path;
      int pos = path.IndexOf("/");
      modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
      return modifiedPath;
    }
  //----< respond to mouse double-click on dir name for navigation>---------------
        private void txtpath_TextChanged(object sender, TextChangedEventArgs e)
        {
            lstfilesn.Items.Clear();
            lstfilesn.Items.Add("[..]");
            var dirs = Directory.GetDirectories(WDirectory);
            foreach (var dir in dirs)
            {
                string dirName = "[" + Path.GetFileName(dir) + "]";

                lstfilesn.Items.Add(dirName);

            }
            List<string> filesMatchingPatterns = new List<string>();
            if (Patterns != null)
            {
                var patterns = Patterns.Split(' ');            //matching the files using the given patterns

                foreach (var patt in patterns)
                    filesMatchingPatterns.AddRange(Directory.GetFiles(WDirectory, patt));
            }
            else
            {
                filesMatchingPatterns = Directory.GetFiles(WDirectory).ToList();
            }
            foreach (var file in filesMatchingPatterns) lstfilesn.Items.Add(Path.GetFileName(file));
        }
 //----<REGEX input from GUI >----------------
        private void txtRegex_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                Regex = txtRegex.Text;
           
            }
        }
 //----<Button Convert Files to display results>----------------
        private void btnPublish_Click(object sender, RoutedEventArgs e)
        {
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            string[] args = Environment.GetCommandLineArgs();
            WDirectory = "../../../";
            WDirectory = Path.GetFullPath(WDirectory);
            List<string> list_st = new List<string>();
            list_st.Add("WpfApp1");
            list_st.Add(WDirectory);
            list_st.Add("/s");
            list_st.AddRange(Patterns.Split(' '));
            list_st.AddRange(Regex.Split(' '));
            CsMessage msg_conv2 = new CsMessage();
            msg_conv2.add("to", CsEndPoint.toString(serverEndPoint));
            msg_conv2.add("from", CsEndPoint.toString(endPoint_));
            msg_conv2.add("command", "convert");             //message sent to convert
            msg_conv2.add("convert", args.First());
            msg_conv2.add("argc", list_st.Count().ToString());
            int count = 1;
            string cmd = "cmd";
            foreach (string val_store in list_st)
            {
                string listch = cmd + count;
                msg_conv2.add(listch, val_store.ToString());
                count++;
            }
            translater.postMessage(msg_conv2);
            CsMessage msgF3 = new CsMessage();
            msgF3.add("to", CsEndPoint.toString(serverEndPoint));
            msgF3.add("from", CsEndPoint.toString(endPoint_));
            msgF3.add("command", "verbose");
            translater.postMessage(msgF3);
        }

  //----< Browse Button to view folders >----------------
        private void btnBrowse_Click(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.FolderBrowserDialog dialog;
            dialog = new System.Windows.Forms.FolderBrowserDialog();
            if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                WDirectory = dialog.SelectedPath;
                txtpath.Text = WDirectory;
            }
        }

  //----< respond to mouse double-click on  converted files sends file to client folder>----------------
        private void listConverted_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (listConverted.SelectedIndex == -1) return;
            string selectedItem = listConverted.SelectedItem.ToString();
            string webpagepathn = selectedItem;
            string npath = "..\\..\\";      // to find the correct path
            npath = npath + webpagepathn;
            System.Diagnostics.Process.Start(npath);
            selectedItem = selectedItem.Substring(21, selectedItem.Length - 21);
            Clientlist.Items.Add(selectedItem);                               //sending file to client on doubleclick
            CsEndPoint serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";
            serverEndPoint.port = 8080;
            CsMessage msgF = new CsMessage();                 //file being transfered to the client folder 
            msgF.add("to", CsEndPoint.toString(serverEndPoint));
            msgF.add("from", CsEndPoint.toString(endPoint_));
            msgF.add("command", "getFile");                  //using message getfile to transfer file to client folder
            msgF.add("name", selectedItem);
            translater.postMessage(msgF);
            CsMessage msgF2 = new CsMessage();
            msgF2.add("to", CsEndPoint.toString(serverEndPoint));
            msgF2.add("from", CsEndPoint.toString(endPoint_));
            msgF2.add("command", "verbose");
            translater.postMessage(msgF2);
        }

        //----< To input text pattern from GUI>----------------
        private void txtPatterns_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                Patterns = txtPatterns.Text;
            }
        }
        //----< respond to mouse double-click on directories >----------------
        private void lstfilesn_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {

            if (lstfilesn.SelectedIndex == -1) return;
            string selectedItem = lstfilesn.SelectedItem.ToString();
            selectedItem = selectedItem.Substring(1, selectedItem.Length - 2);
            selectedItem = Path.Combine(WDirectory, selectedItem);
            selectedItem = Path.GetFullPath(selectedItem);               //returns the full path of the selected item
            if (!Directory.Exists(selectedItem)) return;
            WDirectory = selectedItem;
            txtpath.Text = selectedItem;

        }
        //----< respond to mouse double-click on Client files for download>----------------
        private void Clientlist_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (Clientlist.SelectedIndex == -1) return;
            string selectedItem = Clientlist.SelectedItem.ToString();
            string webpagepathn = selectedItem;
            string npath = "..\\..\\..\\ClientFolder\\";                  
            npath = npath + webpagepathn;
            System.Diagnostics.Process.Start(npath);                   //pops browser from the selected Client files
        }
    }
}
