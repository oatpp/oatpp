import { useState } from "react";
import { Copy, Check, Play } from "lucide-react";
import { cn } from "@/lib/utils";

interface CodeBlockProps {
  code: string;
  language?: string;
  title?: string;
  showLineNumbers?: boolean;
  showPlayground?: boolean;
}

export const CodeBlock = ({
  code,
  language = "cpp",
  title,
  showLineNumbers = false,
  showPlayground = false,
}: CodeBlockProps) => {
  const [copied, setCopied] = useState(false);

  const handleCopy = async () => {
    await navigator.clipboard.writeText(code);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  const lines = code.split("\n");

  return (
    <div className="playground-container my-4">
      {title && (
        <div className="px-4 py-2 bg-muted border-b border-border text-sm font-medium flex items-center justify-between">
          <span>{title}</span>
          <span className="text-xs text-muted-foreground">{language}</span>
        </div>
      )}
      <div className="relative">
        <pre className={cn("oat-code-block", showLineNumbers && "pl-12")}>
          {showLineNumbers ? (
            <code>
              {lines.map((line, i) => (
                <div key={i} className="table-row">
                  <span className="table-cell pr-4 text-oat-gray select-none text-right">
                    {i + 1}
                  </span>
                  <span className="table-cell">{line}</span>
                </div>
              ))}
            </code>
          ) : (
            <code>{code}</code>
          )}
        </pre>
        <div className="absolute top-2 right-2 flex gap-1">
          {showPlayground && (
            <button
              className="copy-button"
              title="Run in playground"
            >
              <Play className="w-4 h-4" />
            </button>
          )}
          <button
            onClick={handleCopy}
            className="copy-button"
            title="Copy to clipboard"
          >
            {copied ? (
              <Check className="w-4 h-4 text-green-500" />
            ) : (
              <Copy className="w-4 h-4" />
            )}
          </button>
        </div>
      </div>
    </div>
  );
};

export default CodeBlock;
