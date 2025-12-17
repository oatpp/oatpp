import { useState } from "react";
import { ThumbsUp, ThumbsDown, MessageSquare } from "lucide-react";
import { toast } from "@/hooks/use-toast";

export const FeedbackWidget = () => {
  const [feedbackGiven, setFeedbackGiven] = useState(false);
  const [showForm, setShowForm] = useState(false);
  const [feedback, setFeedback] = useState("");

  const handleFeedback = (positive: boolean) => {
    setFeedbackGiven(true);
    toast({
      title: "Thanks for your feedback!",
      description: positive 
        ? "We're glad this was helpful." 
        : "We'll work on improving this.",
    });
  };

  const handleSubmitFeedback = () => {
    if (feedback.trim()) {
      toast({
        title: "Feedback submitted",
        description: "Thank you for helping us improve!",
      });
      setFeedback("");
      setShowForm(false);
    }
  };

  if (feedbackGiven && !showForm) {
    return (
      <div className="border-t border-border pt-6 mt-8">
        <p className="text-sm text-muted-foreground">
          Thanks for your feedback!{" "}
          <button
            onClick={() => setShowForm(true)}
            className="oat-link"
          >
            Add more details
          </button>
        </p>
      </div>
    );
  }

  return (
    <div className="border-t border-border pt-6 mt-8">
      {!showForm ? (
        <div className="flex items-center gap-4">
          <span className="text-sm text-muted-foreground">Was this page helpful?</span>
          <div className="flex gap-2">
            <button
              onClick={() => handleFeedback(true)}
              className="btn-outline flex items-center gap-1.5 text-sm"
            >
              <ThumbsUp className="w-4 h-4" />
              Yes
            </button>
            <button
              onClick={() => handleFeedback(false)}
              className="btn-outline flex items-center gap-1.5 text-sm"
            >
              <ThumbsDown className="w-4 h-4" />
              No
            </button>
          </div>
        </div>
      ) : (
        <div className="space-y-3">
          <label className="text-sm font-medium">How can we improve this page?</label>
          <textarea
            value={feedback}
            onChange={(e) => setFeedback(e.target.value)}
            placeholder="Share your suggestions..."
            className="w-full p-3 border border-border rounded-md bg-background text-sm min-h-24 focus:outline-none focus:ring-2 focus:ring-oat-teal/20 focus:border-oat-teal"
          />
          <div className="flex gap-2">
            <button
              onClick={handleSubmitFeedback}
              className="btn-primary"
            >
              Submit Feedback
            </button>
            <button
              onClick={() => setShowForm(false)}
              className="btn-outline"
            >
              Cancel
            </button>
          </div>
        </div>
      )}
    </div>
  );
};

export default FeedbackWidget;
