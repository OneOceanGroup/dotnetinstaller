using System.ComponentModel;

namespace InstallerLib
{
    /// <summary>
    /// InstalledCheck of type "wus".
    /// </summary>
    public class InstalledCheckWindowsUpdate : InstalledCheck
    {
        private string m_knowledgeBaseId;
        private bool m_defaultValue;

        public InstalledCheckWindowsUpdate() : base("check_wus")
        {
            this.KnowledgeBaseId = "KB123456789";
            this.DefaultValue = false;
        }

        /// <summary>
        /// Gets or sets the knowledge base ID of the QFE hotfix.
        /// </summary>
        [Description("QFE Knowledge base ID.")]
        [Required]
        public string KnowledgeBaseId
        {
            get { return this.m_knowledgeBaseId; }
            set { this.m_knowledgeBaseId = value; }
        }

        /// <summary>
        /// Gets or sets a value indicating the default value for the check.
        /// </summary>
        [Description("Default value for the check.")]
        public bool DefaultValue
        {
            get { return this.m_defaultValue; }
            set { this.m_defaultValue = value; }
        }

        /// <inheritdoc />
        protected override void OnXmlWriteTag(XmlWriterEventArgs e)
        {
            e.XmlWriter.WriteAttributeString("kb_id", this.m_knowledgeBaseId);
            e.XmlWriter.WriteAttributeString("default_value", this.m_defaultValue.ToString());
            base.OnXmlWriteTag(e);
        }

        /// <inheritdoc />
        protected override void OnXmlReadTag(XmlElementEventArgs e)
        {
            ReadAttributeValue(e, "kb_id", ref this.m_knowledgeBaseId);
            ReadAttributeValue(e, "default_value", ref this.m_defaultValue);
            base.OnXmlReadTag(e);
        }
    }
}