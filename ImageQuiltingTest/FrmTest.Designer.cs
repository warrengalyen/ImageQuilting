namespace ImageQuiltingTest
{
    unsafe partial class FrmTest
    {
        /// <summary>
        /// Necessary designer variables.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up all the resources that are being used.
        /// </summary>
        /// <param name="disposing">If the managed resource should be released, true; otherwise false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Method Required for Designer Support
        /// - Do not modify the contents of this method using a code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FrmTest));
            this.CmdOpen = new System.Windows.Forms.Button();
            this.CmdSave = new System.Windows.Forms.Button();
            this.PicDest = new System.Windows.Forms.PictureBox();
            this.PicSrc = new System.Windows.Forms.PictureBox();
            this.LblInfo = new System.Windows.Forms.Label();
            this.CmdProcess = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.TileSize = new System.Windows.Forms.HScrollBar();
            this.label2 = new System.Windows.Forms.Label();
            this.Overlap = new System.Windows.Forms.HScrollBar();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.PicDest)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.PicSrc)).BeginInit();
            this.SuspendLayout();
            // 
            // CmdOpen
            // 
            this.CmdOpen.Location = new System.Drawing.Point(4, 7);
            this.CmdOpen.Name = "CmdOpen";
            this.CmdOpen.Size = new System.Drawing.Size(75, 31);
            this.CmdOpen.TabIndex = 21;
            this.CmdOpen.Text = "Open image";
            this.CmdOpen.UseVisualStyleBackColor = true;
            this.CmdOpen.Click += new System.EventHandler(this.CmdOpen_Click);
            // 
            // CmdSave
            // 
            this.CmdSave.Location = new System.Drawing.Point(85, 7);
            this.CmdSave.Name = "CmdSave";
            this.CmdSave.Size = new System.Drawing.Size(75, 31);
            this.CmdSave.TabIndex = 35;
            this.CmdSave.Text = "Save image";
            this.CmdSave.UseVisualStyleBackColor = true;
            this.CmdSave.Click += new System.EventHandler(this.CmdSave_Click);
            // 
            // PicDest
            // 
            this.PicDest.Image = ((System.Drawing.Image)(resources.GetObject("PicDest.Image")));
            this.PicDest.Location = new System.Drawing.Point(445, 47);
            this.PicDest.Name = "PicDest";
            this.PicDest.Size = new System.Drawing.Size(800, 600);
            this.PicDest.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.PicDest.TabIndex = 34;
            this.PicDest.TabStop = false;
            // 
            // PicSrc
            // 
            this.PicSrc.Image = ((System.Drawing.Image)(resources.GetObject("PicSrc.Image")));
            this.PicSrc.Location = new System.Drawing.Point(47, 247);
            this.PicSrc.Name = "PicSrc";
            this.PicSrc.Size = new System.Drawing.Size(320, 345);
            this.PicSrc.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            this.PicSrc.TabIndex = 33;
            this.PicSrc.TabStop = false;
            // 
            // LblInfo
            // 
            this.LblInfo.AutoSize = true;
            this.LblInfo.Location = new System.Drawing.Point(44, 61);
            this.LblInfo.Name = "LblInfo";
            this.LblInfo.Size = new System.Drawing.Size(25, 13);
            this.LblInfo.TabIndex = 36;
            this.LblInfo.Text = "      ";
            // 
            // CmdProcess
            // 
            this.CmdProcess.Location = new System.Drawing.Point(166, 7);
            this.CmdProcess.Name = "CmdProcess";
            this.CmdProcess.Size = new System.Drawing.Size(75, 31);
            this.CmdProcess.TabIndex = 37;
            this.CmdProcess.Text = "Process";
            this.CmdProcess.UseVisualStyleBackColor = true;
            this.CmdProcess.Click += new System.EventHandler(this.CmdDeal_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(270, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(58, 13);
            this.label1.TabIndex = 38;
            this.label1.Text = "Block size:";
            // 
            // TileSize
            // 
            this.TileSize.Location = new System.Drawing.Point(331, 15);
            this.TileSize.Maximum = 128;
            this.TileSize.Minimum = 10;
            this.TileSize.Name = "TileSize";
            this.TileSize.Size = new System.Drawing.Size(237, 17);
            this.TileSize.TabIndex = 39;
            this.TileSize.Value = 50;
            this.TileSize.Scroll += new System.Windows.Forms.ScrollEventHandler(this.TileSize_Scroll);
            this.TileSize.ValueChanged += new System.EventHandler(this.TileSize_ValueChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(585, 20);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(19, 13);
            this.label2.TabIndex = 40;
            this.label2.Text = "50";
            // 
            // Overlap
            // 
            this.Overlap.Location = new System.Drawing.Point(739, 15);
            this.Overlap.Maximum = 64;
            this.Overlap.Minimum = 5;
            this.Overlap.Name = "Overlap";
            this.Overlap.Size = new System.Drawing.Size(242, 17);
            this.Overlap.TabIndex = 42;
            this.Overlap.Value = 10;
            this.Overlap.Scroll += new System.Windows.Forms.ScrollEventHandler(this.Overlap_Scroll);
            this.Overlap.ValueChanged += new System.EventHandler(this.Overlap_ValueChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(627, 20);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(109, 13);
            this.label3.TabIndex = 41;
            this.label3.Text = "Overlapping sections:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(995, 16);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(19, 13);
            this.label4.TabIndex = 43;
            this.label4.Text = "10";
            // 
            // FrmTest
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1257, 706);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.Overlap);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.TileSize);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.CmdProcess);
            this.Controls.Add(this.LblInfo);
            this.Controls.Add(this.CmdSave);
            this.Controls.Add(this.PicDest);
            this.Controls.Add(this.PicSrc);
            this.Controls.Add(this.CmdOpen);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimizeBox = false;
            this.Name = "FrmTest";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Texture synthesis";
            this.Load += new System.EventHandler(this.FrmTest_Load);
            ((System.ComponentModel.ISupportInitialize)(this.PicDest)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.PicSrc)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button CmdOpen;
        private System.Windows.Forms.PictureBox PicSrc;
        private System.Windows.Forms.PictureBox PicDest;
        private System.Windows.Forms.Button CmdSave;
        private System.Windows.Forms.Label LblInfo;
        private System.Windows.Forms.Button CmdProcess;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.HScrollBar TileSize;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.HScrollBar Overlap;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;

    }
}

